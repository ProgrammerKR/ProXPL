
package main

import (
	"fmt"
	"time"
)

type Request struct {
	Method string
	Path   string
}

type Router struct {
	Routes map[string]func() string
}

func NewRouter() *Router {
	return &Router{
		Routes: make(map[string]func() string),
	}
}

func (r *Router) Add(method, path string, handler func() string) {
	key := method + ":" + path
	r.Routes[key] = handler
}

func (r *Router) Dispatch(req Request) string {
	key := req.Method + ":" + req.Path
	if handler, ok := r.Routes[key]; ok {
		return handler()
	}
	return "404 Not Found"
}

func home() string { return "Welcome Home" }
func users() string { return "User List" }
func about() string { return "About Us" }

func main() {
	router := NewRouter()
	router.Add("GET", "/", home)
	router.Add("GET", "/users", users)
	router.Add("GET", "/about", about)

	start := time.Now()

	for i := 0; i < 50000; i++ {
		r1 := Request{Method: "GET", Path: "/"}
		router.Dispatch(r1)

		r2 := Request{Method: "GET", Path: "/users"}
		router.Dispatch(r2)

		r3 := Request{Method: "GET", Path: "/missing"}
		router.Dispatch(r3)
	}

	elapsed := time.Since(start).Seconds()
	fmt.Printf("HTTP Sim Time: %.6f\n", elapsed)
}
