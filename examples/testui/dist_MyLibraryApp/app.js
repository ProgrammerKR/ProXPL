function app() {
  return {
    count: 0,
    increment() {
      this.count = count + 1;
    },
  };
}
