#include "containers/vector.hpp"
#include "tables/rb_tree.hpp"
#include "tables/sorted_array.hpp"
#include "tables/unsorted_array.hpp"
#include "tables/hash_table.hpp"
#include "polynomial/core/polynomial.hpp"
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

namespace sample {

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
        const uint8_t* data2 = reinterpret_cast<const uint8_t*>(data);
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
    using value_type = std::pair<key_type, mapped_type>;
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
    virtual void reset_op_count() noexcept = 0;
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
    void reset_op_count() noexcept override { table_.reset_op_count(); }

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
    void reset_op_count() noexcept override { table_.reset_op_count(); }

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
    void reset_op_count() noexcept override { table_.reset_op_count(); }

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
    void reset_op_count() noexcept override { table_.reset_op_count(); }

private:
    tables::HashTable<key_type, mapped_type, MurMurHash> table_;
};


class ConsoleUIStringPoly {
    using key_t = std::string;
    using value_t = polynomial::Polynomial;
public:

    ConsoleUIStringPoly() {
        tables_.emplace_back(std::make_unique<UnsortedArrayITable<key_t, value_t>>());
        tables_.emplace_back(std::make_unique<SortedArrayITable<key_t, value_t>>());
        tables_.emplace_back(std::make_unique<RBTreeITable<key_t, value_t>>());
        tables_.emplace_back(std::make_unique<HashITable<key_t, value_t>>());
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
    using ptr_table = std::unique_ptr<ITable<key_t, value_t>>;
    containers::Vector<ptr_table> tables_;

    value_t read_poly_from_console() const {
        std::string input;
        std::cout << "Enter a polynomial (e.g., 3x^2 - 2x + 1): ";
        std::getline(std::cin, input);
        try {
            return value_t(input);
        } catch (const std::exception& e) {
            std::cerr << "Invalid polynomial format: " << e.what() << std::endl;
            return value_t();
        }
    }

    key_t read_key_from_console() const {
        key_t key;
        std::cout << "Enter a key: ";
        std::getline(std::cin, key);
        return key;
    }

    void insert_poly(const value_t& poly) {
        auto key = read_key_from_console();
        for (auto& table : tables_) {
            table->insert({ key, poly });
            std::cout << "Inserted into " << table->type_name() << ": " << key << " -> " << poly << std::endl;
            std::cout << "Current size: " << table->size() << ", Operations: " << table->op_count() << std::endl;
            table->reset_op_count();
        }
    }

    void insert_poly() {
        auto key = read_key_from_console();
        insert_poly(read_poly_from_console());
    }

    void find_poly() const {
        auto key = read_key_from_console();
        for (const auto& table : tables_) {
            auto result = table->find(key);
            if (result) {
                std::cout << "Found in " << table->type_name() << ": " << key << " -> " << *result << std::endl;
            } else {
                std::cout << "Not found in " << table->type_name() << ": " << key << std::endl;
            }
            std::cout << "Operations: " << table->op_count() << std::endl;
            table->reset_op_count();
        }
    }

    void remove_poly() {
        auto key = read_key_from_console();
        for (auto& table : tables_) {
            if (table->contains(key)) {
                table->remove(key);
                std::cout << "Removed from " << table->type_name() << ": " << key << std::endl;
            } else {
                std::cout << "Not found in " << table->type_name() << ": " << key << std::endl;
            }
            std::cout << "Operations: " << table->op_count() << std::endl;
            table->reset_op_count();
        }
    }

    double counting_at_point(const value_t& poly) const {
        double res = 0.0;
        auto vars = poly.get_variables();
        if (vars.empty()) {
            for (const auto& monom : poly) {
                res += monom.coefficient();
            }
            return res;
        } else {
            tables::HashTable<uint8_t, double> var_values;
            for (auto var : vars) {
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

    void perform_operation() {
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
                    auto key = read_key_from_console();
                    auto poly_opt = table->find(key);
                    std::cout << "Operations: " << table->op_count() << std::endl;
                    if (poly_opt) {
                        std::cout << "Value at the point: " << counting_at_point(*poly_opt) << std::endl;
                    } else {
                        std::cout << "Polynomial not found for key: " << key << std::endl;
                    }
                    table->reset_op_count();
                } else if (choice == "2" || choice == "3" || choice == "4") {
                    auto key1 = read_key_from_console();
                    auto key2 = read_key_from_console();
                    auto poly1_opt = table->find(key1);
                    auto poly2_opt = table->find(key2);
                    std::cout << "Operations: " << table->op_count() << std::endl;
                    table->reset_op_count();
                    if (poly1_opt && poly2_opt) {
                        value_t p;
                        switch (choice[0]) {
                        case '2':
                            p = std::move(*poly1_opt + *poly2_opt);
                            std::cout << "Result of addition: " << p << std::endl;
                            break;
                        case '3':
                            p = std::move(*poly1_opt - *poly2_opt);
                            std::cout << "Result of subtraction: " << p << std::endl;
                            break;
                        case '4':
                            p = std::move(*poly1_opt * *poly2_opt);
                            std::cout << "Result of multiplication: " << p << std::endl;
                            break;
                        default:
                            std::cout << "Invalid operation choice." << std::endl;
                        }
                        std::cout << "Save the result? (y/n): ";
                        std::string input;
                        std::getline(std::cin, input);
                        if (input == "y" || input == "Y") {
                            insert_poly(p);
                        } else {
                            break;
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
    sample::ConsoleUIStringPoly ui;
    ui.run();
	return 0;
}