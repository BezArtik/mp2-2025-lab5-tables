#include "containers/vector.hpp"
#include "tables/rb_tree.hpp"
#include "tables/sorted_array.hpp"
#include "tables/unsorted_array.hpp"
#include "tables/hash_table.hpp"
#include "polynomial/polynomial.hpp"
#include <iostream>
#include <string>
#include <exception>
#include <optional>
#include <utility>
#include <memory>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <stdexcept>

namespace {

struct MurMurHash {
    size_t operator()(const std::string& key) const noexcept {
        const uint64_t m = 0xc6a4a7935bd1e995;
        const int r = 47;
        uint64_t h = 0x8445d61a4e774912 ^ (key.size() * m);
        const uint64_t* data = reinterpret_cast<const uint64_t*>(key.data());
        const uint64_t* end = data + (key.size() / 8);
        while (data != end) {
            uint64_t k = *data++;
            k *= m;
            k ^= k >> r;
            k *= m;
            h ^= k;
            h *= m;
        }
        const unsigned char* data2 = reinterpret_cast<const unsigned char*>(data);
        switch (key.size() & 7) {
        case 7: h ^= static_cast<uint64_t>(data2[6]) << 48; break;
        case 6: h ^= static_cast<uint64_t>(data2[5]) << 40; break;
        case 5: h ^= static_cast<uint64_t>(data2[4]) << 32; break;
        case 4: h ^= static_cast<uint64_t>(data2[3]) << 24; break;
        case 3: h ^= static_cast<uint64_t>(data2[2]) << 16; break;
        case 2: h ^= static_cast<uint64_t>(data2[1]) << 8;  break;
        case 1: h ^= static_cast<uint64_t>(data2[0]); h *= m;
        }
        h ^= h >> r;
        h *= m;
        h ^= h >> r;
        return static_cast<size_t>(h);
    }
};



template <typename Key, typename Value>
class ITable {
public:
    using key_type = Key;
    using mapped_type = Value;
    using value_type = std::pair<Key, Value>;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    virtual ~ITable() = default;

    virtual void insert(const value_type& value) = 0;
    virtual void insert(value_type&& value) = 0;
    virtual std::optional<mapped_type> find(const key_type& key) const = 0;
    virtual void remove(const key_type& key) = 0;
    virtual bool contains(const key_type& key) const = 0;

    virtual size_type size() const noexcept = 0;
    virtual size_type op_count() const noexcept = 0;
    virtual std::string type_name() const noexcept = 0;
};

template <typename Key, typename Value>
class UnsortedArrayITable : public ITable<Key, Value> {
public:
    using typename ITable<Key, Value>::key_type;
    using typename ITable<Key, Value>::mapped_type;
    using typename ITable<Key, Value>::value_type;
    using typename ITable<Key, Value>::size_type;

    void insert(const value_type& value) override { table_.insert(value); }
    void insert(value_type&& value) override { table_.insert(std::move(value)); }
    std::optional<mapped_type> find(const key_type& key) const override {
        auto it = table_.find(key);
        if (it != table_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    void remove(const key_type& key) override { table_.erase(key); }
    bool contains(const key_type& key) const override { return table_.find(key) != table_.end(); }

    size_type size() const noexcept override { return table_.size(); }
    size_type op_count() const noexcept override { return table_.op_count(); }
    std::string type_name() const noexcept override { return "UnsortedArrayTable"; }

private:
    tables::UnsortedArrayTable<key_type, mapped_type> table_;
};

template <typename Key, typename Value>
class SortedArrayITable : public ITable<Key, Value> {
public:
    using typename ITable<Key, Value>::key_type;
    using typename ITable<Key, Value>::mapped_type;
    using typename ITable<Key, Value>::value_type;
    using typename ITable<Key, Value>::size_type;

    void insert(const value_type& value) override { table_.insert(value); }
    void insert(value_type&& value) override { table_.insert(std::move(value)); }
    std::optional<mapped_type> find(const key_type& key) const override {
        auto it = table_.find(key);
        if (it != table_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    void remove(const key_type& key) override { table_.erase(key); }
    bool contains(const key_type& key) const override { return table_.find(key) != table_.end(); }

    size_type size() const noexcept override { return table_.size(); }
    size_type op_count() const noexcept override { return table_.op_count(); }
    std::string type_name() const noexcept override { return "SortedArrayTable"; }

private:
    tables::SortedArrayTable<key_type, mapped_type> table_;
};

template <typename Key, typename Value>
class RBTreeITable : public ITable<Key, Value> {
public:
    using typename ITable<Key, Value>::key_type;
    using typename ITable<Key, Value>::mapped_type;
    using typename ITable<Key, Value>::value_type;
    using typename ITable<Key, Value>::size_type;

    void insert(const value_type& value) override { table_.insert(value); }
    void insert(value_type&& value) override { table_.insert(std::move(value)); }
    std::optional<mapped_type> find(const key_type& key) const override {
        auto it = table_.find(key);
        if (it != table_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    void remove(const key_type& key) override { table_.erase(key); }
    bool contains(const key_type& key) const override { return table_.find(key) != table_.end(); }

    size_type size() const noexcept override { return table_.size(); }
    size_type op_count() const noexcept override { return table_.op_count(); }
    std::string type_name() const noexcept override { return "RBTreeTable"; }

private:
    tables::RBTreeTable<key_type, mapped_type> table_;
};

template <typename Key, typename Value>
class HashITable : public ITable<Key, Value> {
public:
    using typename ITable<Key, Value>::key_type;
    using typename ITable<Key, Value>::mapped_type;
    using typename ITable<Key, Value>::value_type;
    using typename ITable<Key, Value>::size_type;

    void insert(const value_type& value) override { table_.insert(value); }
    void insert(value_type&& value) override { table_.insert(std::move(value)); }
    std::optional<mapped_type> find(const key_type& key) const override {
        auto it = table_.find(key);
        if (it != table_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    void remove(const key_type& key) override { table_.erase(key); }
    bool contains(const key_type& key) const override { return table_.find(key) != table_.end(); }

    size_type size() const noexcept override { return table_.size(); }
    size_type op_count() const noexcept override { return table_.op_count(); }
    std::string type_name() const noexcept override { return "HashTable"; }

private:
    tables::HashTable<key_type, mapped_type, MurMurHash> table_;
};


class ConsoleUIStringPoly {
public:

    ConsoleUIStringPoly() {
        tables_.emplace_back(std::make_unique<UnsortedArrayITable<std::string, polynomial::Polynomial>>());
        tables_.emplace_back(std::make_unique<SortedArrayITable<std::string, polynomial::Polynomial>>());
        tables_.emplace_back(std::make_unique<RBTreeITable<std::string, polynomial::Polynomial>>());
        tables_.emplace_back(std::make_unique<HashITable<std::string, polynomial::Polynomial>>());
    }

    void run() {
        while (true) {
            std::cout << "Main Menu:\n";
            std::cout << "1. Insert polynomial\n";
            std::cout << "2. Find polynomial\n";
            std::cout << "3. Remove polynomial\n";
            std::cout << "4. Perform operation on polynomials\n";
            std::cout << "5. Exit\n";
            std::cout << "Enter your choice: ";
            std::string choice;
            std::getline(std::cin, choice);
            if (choice.empty()) {
                std::cout << "No input provided. Try again." << std::endl;
                continue;
            }
            switch (choice[0]) {
            case '1': insert_poly();       break;
            case '2': find_poly();         break;
            case '3': remove_poly();       break;
            case '4': perform_operation(); break;
            case '5': return;
            default: std::cout << "Invalid choice. Try again." << std::endl;
            }
        }
    }


private:
    using ptr_table = std::unique_ptr<ITable<std::string, polynomial::Polynomial>>;
    containers::Vector<ptr_table> tables_;

    polynomial::Polynomial read_poly_from_console() const {
        std::string input;
        std::cout << "Enter a polynomial (e.g., 3x^2 - 2x + 1): ";
        std::getline(std::cin, input);
        try {
            return polynomial::Polynomial(input);
        } catch (const std::exception& e) {
            std::cerr << "Invalid polynomial format: " << e.what() << std::endl;
            return polynomial::Polynomial();
        }
    }

    void insert_poly() {
        std::string key;
        std::cout << "Enter a key for the polynomial: ";
        std::getline(std::cin, key);
        auto poly = read_poly_from_console();
        for (auto& table : tables_) {
            table->insert({ key, poly });
            std::cout << "Inserted into " << table->type_name() << ": " << key << " -> " << poly << std::endl;
            std::cout << "Current size: " << table->size() << ", Operations: " << table->op_count() << std::endl;
        }
    }

    void find_poly() const {
        std::string key;
        std::cout << "Enter a key to find the polynomial: ";
        std::getline(std::cin, key);
        for (const auto& table : tables_) {
            auto result = table->find(key);
            if (result) {
                std::cout << "Found in " << table->type_name() << ": " << key << " -> " << *result << std::endl;
            } else {
                std::cout << "Not found in " << table->type_name() << ": " << key << std::endl;
            }
            std::cout << "Operations: " << table->op_count() << std::endl;
        }
    }

    void remove_poly() {
        std::string key;
        std::cout << "Enter a key to remove the polynomial: ";
        std::getline(std::cin, key);
        for (auto& table : tables_) {
            if (table->contains(key)) {
                table->remove(key);
                std::cout << "Removed from " << table->type_name() << ": " << key << std::endl;
            } else {
                std::cout << "Not found in " << table->type_name() << ": " << key << std::endl;
            }
            std::cout << "Operations: " << table->op_count() << std::endl;
        }
    }

    double counting_at_point(const polynomial::Polynomial& poly) const {
        double res = 0.0;
        auto vars = poly.get_variables();
        if (vars.empty()) {
            for (const auto& monom : poly) {
                res += monom.coefficient();
            }
            return res;
        } else {
            tables::HashTable<uint8_t, double> var_values;
            for (uint8_t var : vars) {
                std::cout << "Enter value for variable '" << var << "': ";
                std::string val;
                std::getline(std::cin, val);
                try {
                    double num = std::stod(val);
                    var_values.insert({ var, num });
                } catch (const std::invalid_argument&) {
                    std::cerr << "Invalid value for variable '" << var << "'. Assuming 0." << std::endl;
                    var_values.insert({ var, 0.0 });
                }
            }
            for (const auto& monom : poly) {
                double monom_value = monom.coefficient();
                for (const auto& var : monom) {
                    auto val_opt = var_values.find(var.name_);
                    if (val_opt != var_values.end()) {
                        monom_value *= std::pow(val_opt->second, var.power_);
                    } else {
                        std::cerr << "Value for variable '" << var.name_ << "' not provided. Assuming 0." << std::endl;
                        monom_value = 0;
                        break;
                    }
                }
                res += monom_value;
            }
        }
        return res;
    }

    void perform_operation() const {
        while (true) {
            for (const auto& table : tables_) {
                std::cout << "Possible operations: \n";
                std::cout << "1. Count at a point\n";
                std::cout << "2. Addition\n";
                std::cout << "3. Subtraction\n";
                std::cout << "4. Multiplication\n";
                std::cout << "5. Back to main menu\n";
                std::cout << "Enter your choice: ";
                std::string choice;
                std::getline(std::cin, choice);
                if (choice.empty()) {
                    std::cout << "No input provided. Try again." << std::endl;
                    continue;
                }
                if (choice == "5") {
                    return;
                } else if (choice == "1") {
                    std::string key;
                    std::cout << "Enter a key: ";
                    std::getline(std::cin, key);
                    auto poly_opt = table->find(key);
                    std::cout << "Operations: " << table->op_count() << std::endl;
                    if (poly_opt) {
                        double result = counting_at_point(*poly_opt);
                        std::cout << "Value at the point: " << result << std::endl;
                    } else {
                        std::cout << "Polynomial not found for key: " << key << std::endl;
                    }
                } else if (choice == "2" || choice == "3" || choice == "4") {
                    std::string key1, key2;
                    std::cout << "Enter the first key: ";
                    std::getline(std::cin, key1);
                    std::cout << "Enter the second key: ";
                    std::getline(std::cin, key2);
                    auto poly1_opt = table->find(key1);
                    auto poly2_opt = table->find(key2);
                    std::cout << "Operations: " << table->op_count() << std::endl;
                    if (poly1_opt && poly2_opt) {
                        switch (choice[0]) {
                        case '2':
                            std::cout << "Result of addition: " << *poly1_opt + *poly2_opt << std::endl;
                            break;
                        case '3':
                            std::cout << "Result of subtraction: " << *poly1_opt - *poly2_opt << std::endl;
                            break;
                        case '4':
                            std::cout << "Result of multiplication: " << *poly1_opt * *poly2_opt << std::endl;
                            break;
                        default:
                            std::cout << "Invalid operation choice." << std::endl;
                        }
                    } else {
                        if (!poly1_opt) std::cout << "Polynomial not found for key: " << key1 << std::endl;
                        if (!poly2_opt) std::cout << "Polynomial not found for key: " << key2 << std::endl;
                    }
                } else {
                    std::cout << "Invalid choice. Try again." << std::endl;
                }


            }
        }

    }

};
}

int main() {
    ConsoleUIStringPoly ui;
    ui.run();
	return 0;
}