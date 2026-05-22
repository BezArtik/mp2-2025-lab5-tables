#include "containers/vector.hpp"
#include "tables/sorted_array.hpp"
#include "tables/unsorted_array.hpp"
#include "tables/rb_tree.hpp"
#include "tables/hash_table.hpp"
#include "polynomial/polynomial.hpp"
#include "hash/murmurhash.hpp"
#include <iostream>
#include <string>
#include <exception>
#include <optional>
#include <utility>
#include <memory>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <type_traits>

namespace sample {

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

template <typename Key, typename Value, typename Table>
class TableAdapter : public ITable<Key, Value> {
public:
    using typename ITable<Key, Value>::key_type;
    using typename ITable<Key, Value>::mapped_type;
    using typename ITable<Key, Value>::value_type;
    using typename ITable<Key, Value>::size_type;

    void insert(const value_type& value) override { table_.insert(value); }
    void insert(value_type&& value) override { table_.insert(std::move(value)); }
    std::optional<mapped_type> find(const key_type& key) const override {
        auto it = table_.find(key);
        if (it != table_.end()) return it->second;
        return std::nullopt;
    }
    void remove(const key_type& key) override { table_.erase(key); }
    bool contains(const key_type& key) const override { return table_.find(key) != table_.end(); }

    size_type size() const noexcept override { return table_.size(); }
    size_type op_count() const noexcept override { return table_.op_count(); }
    std::string type_name() const noexcept override { return table_.type_name(); }
    void reset_op_count() noexcept override { table_.reset_op_count(); }

private:
    Table table_;
};

template <typename Key, typename Value>
using UnsortedArrayAdapter = TableAdapter<Key, Value,
    tables::UnsortedArrayTable<Key, Value>
>;

template <typename Key, typename Value>
using SortedArrayAdapter = TableAdapter<Key, Value,
    tables::SortedArrayTable<Key, Value>
>;

template <typename Key, typename Value,
    typename Compare = std::less<Key>>
    using RBTreeAdapter = TableAdapter<Key, Value,
    tables::RBTreeTable<Key, Value, Compare>
    >;

template <typename Key, typename Value,
    typename Hash = std::hash<Key>,
    typename KeyEqual = std::equal_to<Key>>
    using HashAdapter = TableAdapter<Key, Value,
    tables::HashTable<Key, Value, Hash, KeyEqual>
    >;

    
class ConsoleUIStringPoly {
    using key_t = std::string;
    using value_t = polynomial::Polynomial;
public:

    ConsoleUIStringPoly() {
        tables_.emplace_back(std::make_unique<UnsortedArrayAdapter<key_t, value_t>>());
        tables_.emplace_back(std::make_unique<SortedArrayAdapter<key_t, value_t>>());
        tables_.emplace_back(std::make_unique<RBTreeAdapter<key_t, value_t>>());
        tables_.emplace_back(std::make_unique<HashAdapter<key_t, value_t, hash::MurMurHash>>());
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

    void insert_poly(const value_t& poly, const key_t& key) {
        for (auto& table : tables_) {
            table->insert({ key, poly });
            std::cout << "Inserted into " << table->type_name() << ": " << key << " -> " << poly << std::endl;
            std::cout << "Current size: " << table->size() << ", Operations: " << table->op_count() << std::endl;
            table->reset_op_count();
        }
    }

    void insert_poly(const value_t& poly) {
        auto key = read_key_from_console();
        insert_poly(poly, key);
    }

    void insert_poly() {
        auto key = read_key_from_console();
        auto poly = read_poly_from_console();
        insert_poly(poly, key);
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