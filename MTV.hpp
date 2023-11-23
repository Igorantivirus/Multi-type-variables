#pragma once
#include<iostream>
#include<string>
#include<vector>

namespace sib
{
    /*
    Multi type variables
    */
    class MTV
    {
    private:

    #pragma region Base class

        class Base
        {
        public:
            ~Base() {}
            virtual Base* Clone() const = 0;
            virtual std::ostream& print(std::ostream& out) const = 0;
            virtual const std::type_info& getType() const = 0;
            virtual std::string getString() const = 0;
            virtual long double getDouble() const = 0;
            virtual long long getInt() const = 0;
            virtual unsigned long long getUInt() const = 0;
        };

        template<class type>
        class Derived : public Base
        {
        private:
            template<typename type>
            struct HasCout
            {
            private:
                template<typename Stream, typename = decltype(std::declval<Stream&>() << std::declval<const type&>())>
                static std::true_type test(decltype(nullptr) val)
                {
                    return true;
                }
                template<typename>
                static std::false_type test(...)
                {
                    return false;
                }
            public:
                static constexpr bool value = std::is_same<decltype(test<std::ostream>(nullptr)), std::true_type>::value;
            };
        public:
            Derived(type value) : _data(value) {}
            Derived* Clone() const override
            {
                return new Derived(*this);
            }
            std::ostream& print(std::ostream& out) const override
            {
                if constexpr (HasCout<type>::value)
                    out << _data;
                return out;
            }
            const std::type_info& getType() const override
            {
                return typeid(type);
            }
            type getValue()
            {
                return _data;
            }
            std::string getString() const override
            {
                if constexpr (std::is_same_v<type, std::string> || std::is_same_v<type, const char*>)
                    return std::string(_data);
                else if constexpr (std::is_integral_v<type> || std::is_floating_point_v<type> || std::is_same_v<type, bool>)
                    return std::to_string(_data);
                else if constexpr (std::is_same_v<type, char> || std::is_same_v<type, unsigned char> || std::is_same_v<type, wchar_t>)
                    return std::string(1, _data);
                else
                    return "Unsupported Type";
            }
            long double getDouble() const override
            {
                if constexpr (std::is_same_v<type, std::string> || std::is_same_v<type, const char*>)
                    return std::stold(_data);
                else if constexpr (std::is_integral_v<type> || std::is_floating_point_v<type> || std::is_same_v<type, bool>)
                    return static_cast<long double>(_data);
                else
                    return 0.l;
            }
            long long getInt() const override
            {
                if constexpr (std::is_same_v<type, std::string> || std::is_same_v<type, const char*>)
                    return std::stoll(_data);
                else if constexpr (std::is_integral_v<type> || std::is_floating_point_v<type> || std::is_same_v<type, bool>)
                    return static_cast<long long>(_data);
                else
                    return 0;
            }
            unsigned long long getUInt() const override
            {
                if constexpr (std::is_same_v<type, std::string> || std::is_same_v<type, const char*>)
                    return std::stoull(_data);
                else if constexpr (std::is_integral_v<type> || std::is_floating_point_v<type> || std::is_same_v<type, bool>)
                    return static_cast<unsigned long long>(_data);
                else
                    return 0;
            }
        private:
            type _data;
        };

    #pragma endregion

    public:

    #pragma region Constructors Destructors

        MTV() {};
        template<class type>
        MTV(type value)
        {
            _data.push_back(new Derived<type>(value));
        }
        template<class ... types>
        MTV(types ... values)
        {
            push_back(values ...);
        }
        MTV(const MTV& other)
        {
            for (const Base* element : other._data)
                _data.push_back(element->Clone());
        }
        ~MTV()
        {
            for (size_t i = 0; i < _data.size(); i++)
                delete _data[i];
            _data.clear();
            _data.~vector();
        }

    #pragma endregion

        friend std::ostream& operator<<(std::ostream& out, const MTV& mtv)
        {
            if (mtv._data.size() == 0)
                return out;
            if (mtv._data.size() == 1)
                return mtv._data[0]->print(out);
            out << "{ ";
            mtv._data[0]->print(out);
            for (size_t i = 1; i < mtv._data.size(); i++)
            {
                out << ", ";
                mtv._data[i]->print(out);
            }
            return out << " }";
        }
        const type_info& getType()
        {
            if(_data.size() > 1)
                throw std::runtime_error("Value is array");
            return (_data.size() == 0) ? typeid(void) : _data[0]->getType();
        }

    #pragma region Array methods
        size_t size() const
        {
            return _data.size();
        }
        size_t capacity() const
        {
            return _data.capacity();
        }
        void resize(size_t newSize)
        {
            _data.resize(newSize);
        }
        template<class type>
        void resize(size_t newSize, const type& value)
        {
            size_t oldSize = _data.size();
            _data.resize(newSize);
            for (size_t i = oldSize; i < newSize; i++)
            {
                _data[i] = new Derived<type>(value);
            }
        }
        template<class type>
        void push_back(type value)
        {
            _data.push_back(new Derived<type>(value));
        }
        template<class type, class ... types>
        void push_back(type value, types ... values)
        {
            push_back(value);
            push_back(values...);
        }
        void erase(size_t begin_erise)
        {
            for (size_t i = begin_erise; i < _data.size(); i++)
            {
                delete _data[i];
            }
            _data.erase(_data.begin() + begin_erise);
        }
        void erase(size_t begin_erise, size_t count)
        {
            for (size_t i = begin_erise; i < _data.size() && i < (begin_erise + count); i++)
            {
                delete _data[i];
            }
            _data.erase(_data.begin() + begin_erise, _data.begin() + begin_erise + count);
        }
        template<class type>
        type getAt(long long index) const
        {
            if (index >= static_cast<long long>(_data.size()) || index < -static_cast<long long>(_data.size()))
            {
                throw std::out_of_range("Index out of range");
            }
            index = (_data.size() + index) % _data.size();
            if constexpr (std::is_same<type, std::string>::value || std::is_same<type, const char*>::value || std::is_same<type, char*>::value) {
                return _data[index]->getString();
            }
            else if constexpr (std::is_signed<type>::value) {
                return _data[index]->getInt();
            }
            else if constexpr (std::is_unsigned<type>::value) {
                return _data[index]->getUInt();
            }
            else if constexpr (std::is_floating_point<type>::value) {
                return _data[index]->getDouble();
            }
            Derived<type>* valuePtr = dynamic_cast<Derived<type>*>(_data[index]);
            if (valuePtr) {
                return valuePtr->getValue();
            }
            return type();
        }
        template<class type>
        void setAt(long long index, type value)
        {
            if (index >= static_cast<long long>(_data.size()) || index < -static_cast<long long>(_data.size()))
            {
                throw std::out_of_range("Index out of range");
            }
            index = (_data.size() + index) % _data.size();
            delete _data[index];
            _data[index] = new Derived<type>(value);
        }
        MTV operator[](long long index) const
        {
            if (index >= static_cast<long long>(_data.size()) || index < -static_cast<long long>(_data.size()))
            {
                throw std::out_of_range("Index out of range");
            }
            index = (_data.size() + index) % _data.size();
            if (_data[index]->getType() == typeid(MTV))
            {
                Derived<MTV> pr = *dynamic_cast<Derived<MTV>*>(_data[index]);
                return pr.getValue();
            }
            MTV res;
            res._data.push_back(_data[index]->Clone());
            return res;
        }
        template<class type>
        void operator()(long long index, type value)
        {
            if (index >= static_cast<long long>(_data.size()) || index < -static_cast<long long>(_data.size()))
            {
                throw std::out_of_range("Index out of range");
            }
            index = (_data.size() + index) % _data.size();
            delete _data[index];
            _data[index] = new Derived<type>(value);
        }
    #pragma endregion

    #pragma region Operator =
        template<class type>
        MTV& operator=(type value)
        {
            for (size_t i = 0; i < _data.size(); i++)
            {
                delete _data[i];
            }
            _data.clear();
            _data.push_back(new Derived<type>(value));
            return *this;
        }
        MTV& operator=(const MTV& other)
        {
            for (size_t i = 0; i < _data.size(); i++)
            {
                delete _data[i];
            }
            _data.clear();
            for (const Base* element : other._data)
            {
                _data.push_back(element->Clone());
            }
            return *this;
        }
    #pragma endregion

    #pragma region Convertion
        template<class type>
        operator type() const
        {
            ErrorIfArrayOrEmpty();
            if (_data[0]->getType() == typeid(type))
                return dynamic_cast<Derived<type>*>(_data[0])->getValue();
            if constexpr (std::is_same<type, std::string>::value)
                return _data[0]->getString();
            else if constexpr (std::is_floating_point<type>::value)
                return static_cast<type>(_data[0]->getDouble());
            else if constexpr (std::is_signed<type>::value)
                return static_cast<type>(_data[0]->getInt());
            else if constexpr (std::is_unsigned<type>::value)
                return static_cast<type>(_data[0]->getUInt());
            else if constexpr (std::is_same<type, const char*>::value || std::is_same<type, char*>::value)
                return _data[0]->getString().c_str();
            Derived<type>* valuePtr = dynamic_cast<Derived<type>*>(_data[0]);
            if (valuePtr)
                return valuePtr->getValue();
            return type();
        }
        operator std::string() const {
            ErrorIfArrayOrEmpty();
            if (_data[0]->getType() == typeid(std::string))
            {
                return dynamic_cast<Derived<std::string>*>(_data[0])->getValue();
            }
            return _data[0]->getString();
        }
    #pragma endregion

    #pragma region Operator + +=
        template<class type>
        MTV operator+(const type& value) const
        {
            ErrorIfArrayOrEmpty();
            return MTV(type(*this) + value);
        }
        MTV operator+(const std::string& value) const
        {
            ErrorIfArrayOrEmpty();
            return MTV(_data[0]->getString() + value);
        }
        MTV operator+(const MTV& mtv) const
        {
            MTV res = *this;
            for (size_t i = 0; i < mtv._data.size(); i++)
            {
                res._data.push_back(mtv._data[i]->Clone());
            }
            return res;
        }
        template<class type>
        MTV& operator+=(const type& value)
        {
            ErrorIfArrayOrEmpty();
            type pr = type(*this) + value;
            delete _data[0];
            _data[0] = new Derived<type>(pr);
            return *this;
        }
        MTV& operator+=(const std::string& value)
        {
            ErrorIfArrayOrEmpty();
            const std::string pr = _data[0]->getString() + value;
            delete _data[0];
            _data[0] = new Derived<std::string>(pr);
            return *this;
        }
        MTV& operator+=(const MTV& mtv)
        {
            for (size_t i = 0; i < mtv._data.size(); i++)
            {
                _data.push_back(mtv._data[i]->Clone());
            }
            return *this;
        }

        template<class type>
        friend MTV operator+(const type& value, const MTV& mtv)
        {
            mtv.ErrorIfArrayOrEmpty();
            return MTV(value + type(mtv));
        }
        friend MTV operator+(const std::string& value, const MTV& mtv)
        {
            mtv.ErrorIfArrayOrEmpty();
            return MTV(value + mtv._data[0]->getString());
        }
        template<class type>
        friend type& operator+=(type& value, const MTV& mtv)
        {
            mtv.ErrorIfArrayOrEmpty();
            return (value += type(mtv));
        }
        friend std::string& operator+=(std::string& value, const MTV& mtv)
        {
            mtv.ErrorIfArrayOrEmpty();
            return value += mtv._data[0]->getString();
        }
    #pragma endregion

    #pragma region Operator - -=
        template<class type>
        MTV operator-(const type& value) const
        {
            ErrorIfArrayOrEmpty();
            return MTV(type(*this) - value);
        }
        template<class type>
        MTV operator-=(const type& value) const
        {
            ErrorIfArrayOrEmpty();
            type pr = type(*this) - value;
            delete _data[0];
            _data[0] = new Derived<type>(pr);
            return *this;
        }
        template<class type>
        friend MTV operator-(const type& value, const MTV& mtv)
        {
            mtv.ErrorIfArrayOrEmpty();
            return MTV(value - type(mtv));
        }
        template<class type>
        friend type& operator-=(type& value, const MTV& mtv)
        {
            mtv.ErrorIfArrayOrEmpty();
            return (value -= type(mtv));
        }
    #pragma endregion

    #pragma region Operator * *=
        template<class type>
        MTV operator*(const type& value) const
        {
            ErrorIfArrayOrEmpty();
            return MTV(type(*this) * value);
        }
        template<class type>
        MTV& operator*=(const type& value)
        {
            ErrorIfArrayOrEmpty();
            type pr = type(*this) * value;
            delete _data[0];
            _data[0] = new Derived<type>(pr);
            return *this;
        }
        template<class type>
        friend MTV operator*(const type& value, const MTV& mtv)
        {
            mtv.ErrorIfArrayOrEmpty();
            return MTV(value * type(mtv));
        }
        template<class type>
        friend type& operator*=(type& value, const MTV& mtv)
        {
            mtv.ErrorIfArrayOrEmpty();
            return (value *= type(mtv));
        }
    #pragma endregion

    #pragma region Operator / /=
        template<class type>
        MTV operator/(const type& value) const
        {
            ErrorIfArrayOrEmpty();
            return MTV(type(*this) / value);
        }
        template<class type>
        MTV operator/=(const type& value)
        {
            ErrorIfArrayOrEmpty();
            type pr = type(*this) / value;
            delete _data[0];
            _data[0] = new Derived<type>(pr);
            return *this;
        }
        template<class type>
        friend MTV operator/(const type& value, const MTV& mtv)
        {
            mtv.ErrorIfArrayOrEmpty();
            return MTV(value / type(mtv));
        }
        template<class type>
        friend type& operator/=(type& value, const MTV& mtv)
        {
            mtv.ErrorIfArrayOrEmpty();
            return (value /= type(mtv));
        }
    #pragma endregion

    #pragma region Operator % %=
        template<class type>
        MTV operator%(const type& value) const
        {
            ErrorIfArrayOrEmpty();
            return MTV(type(*this) % value);
        }
        template<class type>
        MTV operator%=(const type& value)
        {
            ErrorIfArrayOrEmpty();
            type pr = type(*this) % value;
            delete _data[0];
            _data[0] = new Derived<type>(pr);
            return *this;
        }
        template<class type>
        friend MTV operator%(const type& value, const MTV& mtv)
        {
            mtv.ErrorIfArrayOrEmpty();
            return MTV(value % type(mtv));
        }
        template<class type>
        friend type& operator%=(type& value, const MTV& mtv)
        {
            mtv.ErrorIfArrayOrEmpty();
            return (value %= type(mtv));
        }
    #pragma endregion

    #pragma region Operator << <<=
        template<class type>
        MTV operator<<(const type& value) const
        {
            ErrorIfArrayOrEmpty();
            return type(*this) << value;
        }
        template<class type>
        MTV& operator<<=(const type& value)
        {
            ErrorIfArrayOrEmpty();
            type pr = type(*this) << value;
            delete _data[0];
            _data[0] = new Derived<type>(pr);
            return *this;
        }

        template<class type>
        friend MTV operator<<(const type& value, const MTV& mtv) {
            mtv.ErrorIfArrayOrEmpty();
            return MTV(value << type(mtv));
        }
        template<class type>
        friend type& operator<<=(type& value, const MTV& mtv) {
            mtv.ErrorIfArrayOrEmpty();
            return value <<= type(mtv);
        }
    #pragma endregion

    #pragma region Operator >> >>=
        template<class type>
        MTV operator>>(const type& value) const
        {
            ErrorIfArrayOrEmpty();
            return type(*this) >> value;
        }
        template<class type>
        MTV& operator>>=(const type& value)
        {
            ErrorIfArrayOrEmpty();
            type pr = type(*this) >> value;
            delete _data[0];
            _data[0] = new Derived<type>(pr);
            return *this;
        }

        template<class type>
        friend MTV operator>>(const type& value, const MTV& mtv) {
            mtv.ErrorIfArrayOrEmpty();
            return MTV(value >> type(mtv));
        }
        template<class type>
        friend type& operator>>=(type& value, const MTV& mtv) {
            mtv.ErrorIfArrayOrEmpty();
            return value >>= type(mtv);
        }
    #pragma endregion

    #pragma region Logistic operators
        template<class type>
        bool operator&&(type value) const
        {
            ErrorIfArrayOrEmpty();
            return type(*this) && value;
        }
        template<class type>
        bool operator||(type value) const
        {
            ErrorIfArrayOrEmpty();
            return type(*this) || value;
        }
        template<class type>
        bool operator!() const
        {
            ErrorIfArrayOrEmpty();
            return !bool(*this);
        }
    #pragma endregion

    #pragma region Bit operators
        template<class type>
        MTV operator~() const
        {
            ErrorIfArrayOrEmpty();
            return ~type(*this);
        }

        template<class type>
        MTV operator&(const type& value) const
        {
            ErrorIfArrayOrEmpty();
            return type(*this) & value;
        }
        template<class type>
        MTV operator|(const type& value) const
        {
            ErrorIfArrayOrEmpty();
            return type(*this) | value;
        }
        template<class type>
        MTV operator^(const type& value) const
        {
            ErrorIfArrayOrEmpty();
            return type(*this) ^ value;
        }

        template<class type>
        MTV& operator&=(const type& value)
        {
            ErrorIfArrayOrEmpty();
            type pr = type(*this) & value;
            delete _data[0];
            _data[0] = new Derived<type>(pr);
            return *this;
        }
        template<class type>
        MTV& operator|=(const type& value)
        {
            ErrorIfArrayOrEmpty();
            type pr = type(*this) | value;
            delete _data[0];
            _data[0] = new Derived<type>(pr);
            return *this;
        }
        template<class type>
        MTV& operator^=(const type& value)
        {
            ErrorIfArrayOrEmpty();
            type pr = type(*this) ^ value;
            delete _data[0];
            _data[0] = new Derived<type>(pr);
            return *this;
        }

        template<class type>
        friend MTV operator&(const type& value, const MTV& mtv)
        {
            mtv.ErrorIfArrayOrEmpty();
            return value & type(mtv);
        }
        template<class type>
        friend MTV operator|(const type& value, const MTV& mtv)
        {
            mtv.ErrorIfArrayOrEmpty();
            return value | type(*this);
        }
        template<class type>
        friend MTV operator^(const type& value, const MTV& mtv)
        {
            mtv.ErrorIfArrayOrEmpty();
            return value ^ type(*this);
        }

        template<class type>
        friend type& operator&=(type& value, const MTV& mtv)
        {
            mtv.ErrorIfArrayOrEmpty();
            return value &= type(mtv);
        }
        template<class type>
        friend type& operator|=(type& value, const MTV& mtv)
        {
            mtv.ErrorIfArrayOrEmpty();
            return value |= type(mtv);
        }
        template<class type>
        friend type& operator^=(type& value, const MTV& mtv)
        {
            mtv.ErrorIfArrayOrEmpty();
            return value ^= type(mtv);
        }
    #pragma endregion

    #pragma region Ñomparison operators
        template<class type>
        bool operator<(const type& value) const
        {
            ErrorIfArrayOrEmpty();
            return (static_cast<type>(*this) < value);
        }
        template<class type>
        bool operator>(const type& value) const
        {
            ErrorIfArrayOrEmpty();
            return (static_cast<type>(*this) > value);
        }
        template<class type>
        bool operator<=(const type& value) const
        {
            return !this->operator>(value);
        }
        template<class type>
        bool operator>=(const type& value) const
        {
            return !this->operator<(value);
        }
        template<class type>
        friend bool operator<(const type& value, const MTV& mtv)
        {
            return mtv > value;
        }
        template<class type>
        friend bool operator>(const type& value, const MTV& mtv)
        {
            return mtv < value;
        }
        template<class type>
        friend bool operator<=(const type& value, const MTV& mtv)
        {
            return mtv >= value;
        }
        template<class type>
        friend bool operator>=(const type& value, const MTV& mtv)
        {
            return mtv <= value;
        }
        template<class type>
        bool operator==(const type& value) const
        {
            if (_data.size() != 1)
            {
                return false;
            }
            return (type(*this) == value) || (_data[0]->getString() == to_string(value));
        }
        bool operator==(const MTV& mtv) const {
            if (_data.size() != mtv._data.size())
            {
                return false;
            }
            for (size_t i = 0; i < _data.size(); i++)
            {
                if (_data[i]->getString() != mtv._data[i]->getString())
                {
                    return false;
                }
            }
            return true;
        }
        template<class type>
        friend bool operator==(const type& value, const MTV& mtv)
        {
            return mtv == value;
        }
    #pragma endregion
    private:
        std::vector<Base*> _data;
        void ErrorIfArrayOrEmpty() const
        {
            if (_data.size() == 0)
                throw std::runtime_error("Value is not initialized");
            if (_data.size() > 1)
                throw std::runtime_error("Value is array");
        }
    };

    typedef MTV Auto;
    typedef MTV var;
}