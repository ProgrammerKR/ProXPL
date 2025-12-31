const fs = require('fs');
const path = require('path');

const fileIconsPath = path.join(__dirname, '../temp_icons_src/src/core/icons/fileIcons.ts');
const languageIconsPath = path.join(__dirname, '../temp_icons_src/src/core/icons/languageIcons.ts');
const iconsDir = path.join(__dirname, 'icons');
const outputPath = path.join(__dirname, 'icons/icon-theme.json');

// Helper to sanitize regex matches
function cleanArray(str) {
    if (!str) return [];
    return str.split(',').map(s => s.trim().replace(/['"]/g, '')).filter(s => s);
}

function parseFileIcons(content) {
    const icons = [];
    // Simple regex to match objects in the array. 
    // This assumes specific formatting in the source file.
    // We look for { name: '...', ... } blocks.
    // Handling multiline is key.
    
    // Normalize string to single line for easier regex (removing comments first)
    const oneline = content.replace(/\/\*[\s\S]*?\*\/|\/\/.*/g, '').replace(/\s+/g, ' ');
    
    // Regex to match objects: { name: 'foo', fileExtensions: [...], fileNames: [...] }
    // Note: This is an approximation.
    const regex = /{ name: '([^']+)'(?:, fileExtensions: \[([^\]]*)\])?(?:, fileNames: \[([^\]]*)\])?(?:, light: true)?(?:, enabledFor: \[([^\]]*)\])?/g;
    
    let match;
    while ((match = regex.exec(oneline)) !== null) {
        icons.push({
            name: match[1],
            extensions: cleanArray(match[2]),
            filenames: cleanArray(match[3])
        });
    }
    return icons;
}

function parseLanguageIcons(content) {
    const icons = [];
    const oneline = content.replace(/\/\*[\s\S]*?\*\/|\/\/.*/g, '').replace(/\s+/g, ' ');
    const regex = /{ name: '([^']+)', ids: \[([^\]]*)\](?:, clone: \{[^}]+\})?/g;
    
    let match;
    while ((match = regex.exec(oneline)) !== null) {
        icons.push({
            name: match[1],
            ids: cleanArray(match[2])
        });
    }
    return icons;
}

const fileIconsContent = fs.readFileSync(fileIconsPath, 'utf8');
const languageIconsContent = fs.readFileSync(languageIconsPath, 'utf8');

const fileIconDefs = parseFileIcons(fileIconsContent);
const langIconDefs = parseLanguageIcons(languageIconsContent);

const iconTheme = {
    iconDefinitions: {
        "_proxpl": { "iconPath": "./proxpl.svg" }
    },
    fileExtensions: {
        "prox": "_proxpl",
        "pxpl": "_proxpl"
    },
    fileNames: {},
    languageIds: {
        "proxpl": "_proxpl"
    }
};

// Process File Icons
fileIconDefs.forEach(def => {
    const iconName = def.name;
    const svgPath = path.join(iconsDir, `${iconName}.svg`);
    
    if (fs.existsSync(svgPath)) {
        const iconId = `_${iconName}`;
        iconTheme.iconDefinitions[iconId] = { "iconPath": `./${iconName}.svg` };
        
        def.extensions.forEach(ext => {
            if (!iconTheme.fileExtensions[ext]) { // Don't overwrite existing
                iconTheme.fileExtensions[ext] = iconId;
            }
        });
        
        def.filenames.forEach(fn => {
            if (!iconTheme.fileNames[fn]) {
                iconTheme.fileNames[fn] = iconId;
            }
        });
    }
});

// Process Language Icons
langIconDefs.forEach(def => {
    const iconName = def.name;
    const svgPath = path.join(iconsDir, `${iconName}.svg`);
    
    // Check if we already defined this icon (likely yes from file icons)
    // If not, verify existence and add definition
    let iconId = `_${iconName}`;
    if (!iconTheme.iconDefinitions[iconId]) {
        if (fs.existsSync(svgPath)) {
            iconTheme.iconDefinitions[iconId] = { "iconPath": `./${iconName}.svg` };
        } else {
            return; // Skip if icon doesn't exist
        }
    }
    
    def.ids.forEach(id => {
        if (!iconTheme.languageIds[id]) {
            iconTheme.languageIds[id] = iconId;
        }
    });
});

fs.writeFileSync(outputPath, JSON.stringify(iconTheme, null, 2));
console.log('Icon theme generated at ' + outputPath);
