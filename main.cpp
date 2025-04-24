#include <iostream>
#include <unordered_map>
#include <list>
#include <map>

using namespace std;

// ------------------- Base Cache Class -------------------
class Cache
{
protected:
    int capacity;

public:
    Cache(int cap) : capacity(cap) {}
    virtual int get(int key) = 0;
    virtual void put(int key, int value) = 0;
    virtual void display() = 0;
    virtual ~Cache() {}
};

// ------------------- LRU Cache -------------------
class LRUCache : public Cache
{
    list<pair<int, int>> dll;
    unordered_map<int, list<pair<int, int>>::iterator> hashmap;

public:
    LRUCache(int cap) : Cache(cap) {}

    int get(int key) override
    {
        if (hashmap.find(key) == hashmap.end())
            return -1;

        auto node = hashmap[key];
        int value = node->second;
        dll.erase(node);
        dll.push_front({key, value});
        hashmap[key] = dll.begin();
        return value;
    }

    void put(int key, int value) override
    {
        if (hashmap.find(key) != hashmap.end())
        {
            dll.erase(hashmap[key]);
        }
        else if (dll.size() == capacity)
        {
            auto last = dll.back();
            hashmap.erase(last.first);
            dll.pop_back();
        }
        dll.push_front({key, value});
        hashmap[key] = dll.begin();
    }

    void display() override
    {
        cout << "LRU Cache: ";
        for (auto it : dll)
        {
            cout << "[" << it.first << ":" << it.second << "] ";
        }
        cout << endl;
    }
};

// ------------------- LFU Cache -------------------
class LFUCache : public Cache
{
    struct LFUNode
    {
        int key, value, freq;
    };

    int minFreq;
    unordered_map<int, LFUNode> keyMap;              // key -> node
    unordered_map<int, list<int>> freqMap;           // freq -> list of keys
    unordered_map<int, list<int>::iterator> iterMap; // key -> iterator in freqMap

    void updateFreq(int key)
    {
        int freq = keyMap[key].freq;
        freqMap[freq].erase(iterMap[key]);

        if (freqMap[freq].empty())
        {
            freqMap.erase(freq);
            if (minFreq == freq)
                minFreq++;
        }

        keyMap[key].freq++;
        freqMap[freq + 1].push_front(key);
        iterMap[key] = freqMap[freq + 1].begin();
    }

public:
    LFUCache(int cap) : Cache(cap), minFreq(0) {}

    int get(int key) override
    {
        if (keyMap.find(key) == keyMap.end())
            return -1;

        updateFreq(key);
        return keyMap[key].value;
    }

    void put(int key, int value) override
    {
        if (capacity == 0)
            return;

        if (keyMap.find(key) != keyMap.end())
        {
            keyMap[key].value = value;
            updateFreq(key);
        }
        else
        {
            if (keyMap.size() == capacity)
            {
                int evictKey = freqMap[minFreq].back();
                freqMap[minFreq].pop_back();
                if (freqMap[minFreq].empty())
                    freqMap.erase(minFreq);
                keyMap.erase(evictKey);
                iterMap.erase(evictKey);
            }

            keyMap[key] = {key, value, 1};
            freqMap[1].push_front(key);
            iterMap[key] = freqMap[1].begin();
            minFreq = 1;
        }
    }

    void display() override
    {
        cout << "LFU Cache:\n";
        for (auto it = freqMap.begin(); it != freqMap.end(); ++it)
        {
            int freq = it->first;
            list<int> &keys = it->second;
            if (!keys.empty())
            {
                cout << "Freq " << freq << ": ";
                for (int key : keys)
                {
                    cout << "[" << key << ":" << keyMap[key].value << "] ";
                }
                cout << "\n";
            }
        }
    }
};

// ------------------- Simulator -------------------
int main()
{
    cout << "Choose cache policy:\n1. LRU\n2. LFU\nChoice: ";
    int choice;
    cin >> choice;

    cout << "Enter cache capacity: ";
    int capacity;
    cin >> capacity;

    Cache *cache;

    if (choice == 1)
        cache = new LRUCache(capacity);
    else
        cache = new LFUCache(capacity);

    while (true)
    {
        cout << "\nOptions: 1. Put  2. Get  3. Display  4. Exit\nEnter option: ";
        int opt;
        cin >> opt;

        if (opt == 1)
        {
            int key, val;
            cout << "Key: ";
            cin >> key;
            cout << "Value: ";
            cin >> val;
            cache->put(key, val);
        }
        else if (opt == 2)
        {
            int key;
            cout << "Key: ";
            cin >> key;
            int res = cache->get(key);
            if (res == -1)
                cout << "Key not found.\n";
            else
                cout << "Value: " << res << "\n";
        }
        else if (opt == 3)
        {
            cache->display();
        }
        else
        {
            break;
        }
    }

    delete cache;
    return 0;
}
