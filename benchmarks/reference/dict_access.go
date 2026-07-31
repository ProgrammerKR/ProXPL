package main
import ("fmt"; "time"; "strconv")
func main() {
    start := time.Now()
    d := make(map[string]int)
    for i := 0; i < 10000; i++ { d["key" + strconv.Itoa(i)] = i }
    totalSum := 0
    for i := 0; i < 10000; i++ { totalSum += d["key" + strconv.Itoa(i)] }
    fmt.Println("Dict Sum:", totalSum)
    fmt.Printf("Time: %.6f\n", time.Since(start).Seconds())
}
