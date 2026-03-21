function app() {
  return {
    count: 0,
    increment() {
      this.count = this.count + 1;
    },
  };
}
