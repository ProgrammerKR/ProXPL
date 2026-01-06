
#include <iostream>
#include <string>
#include <unordered_map>
#include <chrono>
#include <functional>

using namespace std;

class Request {
public:
    string method;
    string path;
    Request(string m, string p) : method(m), path(p) {}
};

using Handler = function<string()>;

class Router {
public:
    unordered_map<string, Handler> routes;

    void add(string method, string path, Handler handler) {
        string key = method + ":" + path;
        routes[key] = handler;
    }

    string dispatch(const Request& req) {
        string key = req.method + ":" + req.path;
        if (routes.find(key) != routes.end()) {
            return routes[key]();
        }
        return "404 Not Found";
    }
};

string home() { return "Welcome Home"; }
string users() { return "User List"; }
string about() { return "About Us"; }

int main() {
    Router router;
    router.add("GET", "/", home);
    router.add("GET", "/users", users);
    router.add("GET", "/about", about);

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < 50000; ++i) {
        Request r1("GET", "/");
        router.dispatch(r1);

        Request r2("GET", "/users");
        router.dispatch(r2);

        Request r3("GET", "/missing");
        router.dispatch(r3);
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "HTTP Sim Time: " << elapsed.count() << endl;

    return 0;
}
