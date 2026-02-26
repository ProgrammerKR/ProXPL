function app() {
  return {
    count: 0,
    increment() {
      this.count++;
      
      // Add a tiny shake effect when maxed somewhat (optional fun detail)
      const el = document.querySelector('.count-display');
      if(el) {
          el.style.transform = 'scale(1.2)';
          el.style.transition = 'transform 0.1s';
          setTimeout(() => {
              el.style.transform = 'scale(1)';
              el.style.transition = 'transform 0.3s cubic-bezier(0.34, 1.56, 0.64, 1)';
          }, 100);
      }
    }
  }
}
