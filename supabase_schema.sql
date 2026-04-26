-- ProXPL Registry Database Schema

-- 1. Profiles table (linked to auth.users)
CREATE TABLE IF NOT EXISTS public.profiles (
    id UUID REFERENCES auth.users ON DELETE CASCADE PRIMARY KEY,
    username TEXT UNIQUE NOT NULL,
    display_name TEXT,
    avatar_url TEXT,
    bio TEXT,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT timezone('utc'::text, now()) NOT NULL,
    updated_at TIMESTAMP WITH TIME ZONE DEFAULT timezone('utc'::text, now()) NOT NULL
);

-- Enable RLS
ALTER TABLE public.profiles ENABLE ROW LEVEL SECURITY;

-- 2. Packages table
CREATE TABLE IF NOT EXISTS public.packages (
    id UUID DEFAULT gen_random_uuid() PRIMARY KEY,
    name TEXT UNIQUE NOT NULL,
    description TEXT,
    author_id UUID REFERENCES public.profiles(id) ON DELETE SET NULL,
    repository_url TEXT,
    homepage_url TEXT,
    license TEXT DEFAULT 'MIT',
    category TEXT,
    total_downloads INTEGER DEFAULT 0,
    stars INTEGER DEFAULT 0,
    is_verified BOOLEAN DEFAULT false,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT timezone('utc'::text, now()) NOT NULL,
    updated_at TIMESTAMP WITH TIME ZONE DEFAULT timezone('utc'::text, now()) NOT NULL
);

-- Enable RLS
ALTER TABLE public.packages ENABLE ROW LEVEL SECURITY;

-- 3. Package Versions table
CREATE TABLE IF NOT EXISTS public.package_versions (
    id UUID DEFAULT gen_random_uuid() PRIMARY KEY,
    package_id UUID REFERENCES public.packages(id) ON DELETE CASCADE,
    version TEXT NOT NULL,
    entry_point TEXT,
    checksum TEXT,
    download_url TEXT NOT NULL,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT timezone('utc'::text, now()) NOT NULL,
    UNIQUE(package_id, version)
);

-- Enable RLS
ALTER TABLE public.package_versions ENABLE ROW LEVEL SECURITY;

-- Policies

-- Profiles: Anyone can read, only owner can update
CREATE POLICY "Public profiles are viewable by everyone." ON public.profiles
    FOR SELECT USING (true);

CREATE POLICY "Users can insert their own profile." ON public.profiles
    FOR INSERT WITH CHECK (auth.uid() = id);

CREATE POLICY "Users can update own profile." ON public.profiles
    FOR UPDATE USING (auth.uid() = id);

-- Packages: Anyone can read, only authenticated can create, owner can update
CREATE POLICY "Packages are viewable by everyone." ON public.packages
    FOR SELECT USING (true);

CREATE POLICY "Authenticated users can create packages." ON public.packages
    FOR INSERT WITH CHECK (auth.role() = 'authenticated');

CREATE POLICY "Owners can update their own packages." ON public.packages
    FOR UPDATE USING (auth.uid() = author_id);

-- Versions: Anyone can read, only package owner can insert
CREATE POLICY "Versions are viewable by everyone." ON public.package_versions
    FOR SELECT USING (true);

CREATE POLICY "Package owners can insert versions." ON public.package_versions
    FOR INSERT WITH CHECK (
        EXISTS (
            SELECT 1 FROM public.packages
            WHERE id = package_id AND author_id = auth.uid()
        )
    );

-- Functions
CREATE OR REPLACE FUNCTION public.handle_new_user()
RETURNS trigger AS $$
BEGIN
  INSERT INTO public.profiles (id, username, display_name, avatar_url)
  VALUES (new.id, new.raw_user_meta_data->>'username', new.raw_user_meta_data->>'username', new.raw_user_meta_data->>'avatar_url');
  RETURN new;
END;
$$ LANGUAGE plpgsql SECURITY DEFINER;

-- Trigger for new user
CREATE OR REPLACE TRIGGER on_auth_user_created
  AFTER INSERT ON auth.users
  FOR EACH ROW EXECUTE PROCEDURE public.handle_new_user();
