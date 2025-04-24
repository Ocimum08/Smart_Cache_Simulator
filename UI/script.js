let cache = null;

class LRUCache {
  constructor(capacity) {
    this.capacity = capacity;
    this.map = new Map();
  }

  get(key) {
    if (!this.map.has(key)) return -1;
    const value = this.map.get(key);
    this.map.delete(key);
    this.map.set(key, value);
    return value;
  }

  put(key, value) {
    if (this.map.has(key)) this.map.delete(key);
    else if (this.map.size >= this.capacity)
      this.map.delete(this.map.keys().next().value);
    this.map.set(key, value);
  }

  display() {
    let result = "LRU Cache: ";
    this.map.forEach((value, key) => {
      result += `[${key}:${value}] `;
    });
    return result;
  }
}

class LFUCache {
  constructor(capacity) {
    this.capacity = capacity;
    this.keyMap = new Map(); // key -> {value, freq}
    this.freqMap = new Map(); // freq -> Set of keys
    this.minFreq = 0;
  }

  updateFreq(key) {
    const node = this.keyMap.get(key);
    const freq = node.freq;
    this.freqMap.get(freq).delete(key);
    if (this.freqMap.get(freq).size === 0) {
      this.freqMap.delete(freq);
      if (this.minFreq === freq) this.minFreq++;
    }

    node.freq++;
    if (!this.freqMap.has(node.freq)) this.freqMap.set(node.freq, new Set());
    this.freqMap.get(node.freq).add(key);
  }

  get(key) {
    if (!this.keyMap.has(key)) return -1;
    this.updateFreq(key);
    return this.keyMap.get(key).value;
  }

  put(key, value) {
    if (this.capacity === 0) return;

    if (this.keyMap.has(key)) {
      this.keyMap.get(key).value = value;
      this.updateFreq(key);
    } else {
      if (this.keyMap.size >= this.capacity) {
        const keyToEvict = this.freqMap.get(this.minFreq).values().next().value;
        this.freqMap.get(this.minFreq).delete(keyToEvict);
        if (this.freqMap.get(this.minFreq).size === 0)
          this.freqMap.delete(this.minFreq);
        this.keyMap.delete(keyToEvict);
      }

      this.keyMap.set(key, { value: value, freq: 1 });
      if (!this.freqMap.has(1)) this.freqMap.set(1, new Set());
      this.freqMap.get(1).add(key);
      this.minFreq = 1;
    }
  }

  display() {
    let result = "LFU Cache:\n";
    for (let [freq, keys] of this.freqMap.entries()) {
      result += `Freq ${freq}: `;
      for (let key of keys) {
        result += `[${key}:${this.keyMap.get(key).value}] `;
      }
      result += "\n";
    }
    return result;
  }
}

function initializeCache() {
  const policy = document.getElementById("policy").value;
  const capacity = parseInt(document.getElementById("capacity").value);
  if (capacity <= 0) {
    alert("Capacity must be greater than 0.");
    return;
  }
  if (policy === "LRU") cache = new LRUCache(capacity);
  else cache = new LFUCache(capacity);
  document.getElementById(
    "output"
  ).innerText = `Initialized ${policy} Cache with capacity ${capacity}`;
}

function put() {
  const key = parseInt(document.getElementById("keyInput").value);
  const value = parseInt(document.getElementById("valueInput").value);
  if (isNaN(key) || isNaN(value)) {
    alert("Enter valid key and value.");
    return;
  }
  cache.put(key, value);
  document.getElementById(
    "output"
  ).innerText = `Inserted key ${key} with value ${value}`;
}

function get() {
  const key = parseInt(document.getElementById("keyInput").value);
  if (isNaN(key)) {
    alert("Enter a valid key.");
    return;
  }
  const val = cache.get(key);
  document.getElementById("output").innerText =
    val === -1 ? "Key not found." : `Value: ${val}`;
}

function display() {
  document.getElementById("output").innerText = cache.display();
}
