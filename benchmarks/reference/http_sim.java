
// package benchmarks.reference;

import java.util.HashMap;
import java.util.Map;
import java.util.function.Supplier;

class Request {
    String method;
    String path;

    public Request(String method, String path) {
        this.method = method;
        this.path = path;
    }
}

class Router {
    Map<String, Supplier<String>> routes = new HashMap<>();

    public void add(String method, String path, Supplier<String> handler) {
        String key = method + ":" + path;
        routes.put(key, handler);
    }

    public String dispatch(Request req) {
        String key = req.method + ":" + req.path;
        if (routes.containsKey(key)) {
            return routes.get(key).get();
        }
        return "404 Not Found";
    }
}

public class http_sim {
    static String home() { return "Welcome Home"; }
    static String users() { return "User List"; }
    static String about() { return "About Us"; }

    public static void main(String[] args) {
        Router router = new Router();
        router.add("GET", "/", http_sim::home);
        router.add("GET", "/users", http_sim::users);
        router.add("GET", "/about", http_sim::about);

        long start = System.nanoTime();

        for (int i = 0; i < 50000; i++) {
            Request r1 = new Request("GET", "/");
            router.dispatch(r1);

            Request r2 = new Request("GET", "/users");
            router.dispatch(r2);

            Request r3 = new Request("GET", "/missing");
            router.dispatch(r3);
        }

        long end = System.nanoTime();
        double elapsed = (end - start) / 1_000_000_000.0;

        System.out.println("HTTP Sim Time: " + elapsed);
    }
}
