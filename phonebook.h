#include <algorithm>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>

std::string NormalizeName(std::string text);

std::string NormalizePhone(std::string phone);

struct Name {
    std::string name_;

    explicit Name(std::string name);
};

struct Phone {
    std::string phone_;

    explicit Phone(std::string phone);
};

struct Address {
    std::optional<std::string> address_;

    explicit Address(std::string str);

    bool operator==(const Address &rhs) const;
};

class Contact {
private:
    int id_;
    Name name_;
    Phone phone_;
    Address address_;

    friend class Phonebook;

    std::string GetNormName() const;

    std::string GetNormPhone() const;

public:
    Contact();

    Contact(int, std::string, std::string, std::string);

    std::string GetName() const;

    std::string GetPhone() const;

    bool NoAddress() const;

    Address GetAddress() const;

    std::string GetStringAddress() const;

    int GetID() const;

    bool operator==(const Contact &rhs) const;
};

class Phonebook {
public:
    Phonebook();

    int AddContact(std::string name, std::string phone, std::string address = "");

    bool Valid(const Contact &contact) const;

    std::string GetName(int id) const;

    std::string GetPhone(int id) const;

    std::optional<std::string> GetAddress(int id) const;

    int FindByName(const std::string &name) const;

    int FindByPhone(const std::string &phone) const;

    std::optional<Contact> FindByID(int id) const;

    bool EraseByID(int id);

    std::vector<Contact> FilterByAddress(std::string substr) const;

    bool SetName(int id, std::string new_name);

    bool SetPhone(int id, std::string new_phone);

    bool SetAddress(int id, std::string new_address);

private:
    std::unordered_map<std::string, int> name_map_;
    std::unordered_map<std::string, int> phone_map_;
    std::unordered_map<int, Contact> contact_map_;
    int id_count_;
};

/**************************************************
 **************************************************
 ************** Р Е А Л И З А Ц И Я ***************
 **************************************************
 **************************************************/

std::string NormalizeName(std::string text) {
    char sep = ' ';
    std::transform(text.begin(), text.end(), text.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    std::vector<std::string> tokens;
    std::size_t start = 0, end = 0;
    while ((end = text.find(sep, start)) != std::string::npos) {
        if (end > start) {
            tokens.push_back(text.substr(start, end - start));
        }
        start = end + 1;
    }
    if (start != text.size()) {
        tokens.push_back(text.substr(start));
    }
    std::sort(tokens.begin(), tokens.end());
    std::string result;
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (i + 1 != tokens.size()) {
            result += tokens[i] + ' ';
        } else {
            result += tokens[i];
        }
    }
    return result;
}

std::string NormalizePhone(std::string phone) {
    if (phone[0] == '+') {
        phone.erase(0, 1);
    }
    return phone;
}

Name::Name(std::string name) : name_(name) {
}

Phone::Phone(std::string phone) : phone_(phone) {
}

Address::Address(std::string str) {
    if (str.empty()) {
        address_ = std::nullopt;
    } else {
        address_ = std::optional<std::string>(str);
    }
}

bool Address::operator==(const Address &rhs) const {
    return address_ == rhs.address_;
}

std::string Contact::GetNormName() const {
    return NormalizeName(name_.name_);
}

std::string Contact::GetNormPhone() const {
    return NormalizePhone(phone_.phone_);
}

Contact::Contact() : id_(-1), name_(Name("")), phone_(Phone("")), address_(Address("")) {
}

Contact::Contact(int id, std::string name, std::string phone, std::string address = "")
    : id_(id), name_(Name(name)), phone_(Phone(phone)), address_(Address(address)) {
}

std::string Contact::GetName() const {
    return name_.name_;
}

std::string Contact::GetPhone() const {
    return phone_.phone_;
}

Address Contact::GetAddress() const {
    return address_;
}

std::string Contact::GetStringAddress() const {
    return address_.address_.value();
}

bool Contact::NoAddress() const {
    return address_.address_ == std::nullopt;
}

int Contact::GetID() const {
    return id_;
}

bool Contact::operator==(const Contact &rhs) const {
    return (id_ == rhs.id_ && NormalizeName(name_.name_) == NormalizeName(rhs.name_.name_) &&
            NormalizePhone(phone_.phone_) == NormalizePhone(rhs.phone_.phone_) &&
            address_ == rhs.GetAddress());
}

Phonebook::Phonebook() {
    name_map_ = std::unordered_map<std::string, int>();
    phone_map_ = std::unordered_map<std::string, int>();
    contact_map_ = std::unordered_map<int, Contact>();
    id_count_ = 0;
}

// id == -1 значит ошибка при совпадении
int Phonebook::AddContact(std::string name, std::string phone, std::string address) {
    std::string norm_name = NormalizeName(name);
    std::string norm_phone = NormalizePhone(phone);
    if (name_map_.find(norm_name) != name_map_.end() ||
        phone_map_.find(norm_phone) != phone_map_.end()) {
        return -1;
    }
    int id = id_count_++;
    name_map_[norm_name] = id;
    phone_map_[norm_phone] = id;
    contact_map_[id] = Contact(id, name, phone, address);
    return id;
}

bool Phonebook::Valid(const Contact &contact) const {
    int id = contact.GetID();
    auto contact_iter = contact_map_.find(id);
    if (contact_iter == contact_map_.end()) {
        return false;
    }
    return contact_iter->second == contact;
}

std::string Phonebook::GetName(int id) const {
    return contact_map_.at(id).GetName();
}

std::string Phonebook::GetPhone(int id) const {
    return contact_map_.at(id).GetPhone();
}

std::optional<std::string> Phonebook::GetAddress(int id) const {
    auto contact = contact_map_.at(id);
    if (!contact.GetAddress().address_) {
        return std::nullopt;
    }
    return contact.GetAddress().address_.value();
}

int Phonebook::FindByName(const std::string &name) const {
    std::string norm_name = NormalizeName(name);
    auto contact_iter = name_map_.find(norm_name);
    if (contact_iter == name_map_.end()) {
        return -1;
    }
    return contact_iter->second;
}

int Phonebook::FindByPhone(const std::string &phone) const {
    std::string norm_phone = NormalizePhone(phone);
    auto contact_iter = phone_map_.find(norm_phone);
    if (contact_iter == phone_map_.end()) {
        return -1;
    }
    return contact_iter->second;
}

std::optional<Contact> Phonebook::FindByID(int id) const {
    auto contact_iter = contact_map_.find(id);
    if (contact_iter == contact_map_.end()) {
        return std::nullopt;
    }
    return contact_iter->second;
}

bool Phonebook::EraseByID(int id) {
    auto contact_iter = contact_map_.find(id);
    if (contact_iter == contact_map_.end()) {
        return false;
    }
    name_map_.erase(contact_iter->second.GetNormName());
    phone_map_.erase(contact_iter->second.GetNormPhone());
    contact_map_.erase(id);
    return true;
}

std::vector<Contact> Phonebook::FilterByAddress(std::string substr) const {
    std::vector<Contact> result;
    for (auto &x : contact_map_) {
        if (!x.second.NoAddress()) {
            std::string x_adr = x.second.GetStringAddress();
            if (x_adr.find(substr) != std::string::npos) {
                result.push_back(x.second);
            }
        }
    }
    return result;
}

bool Phonebook::SetName(int id, std::string new_name) {
    auto contact_iter = contact_map_.find(id);
    if (contact_iter == contact_map_.end()) {
        return false;
    }
    name_map_.erase(contact_iter->second.GetNormName());
    contact_iter->second.name_ = Name(new_name);
    name_map_[contact_iter->second.GetNormName()] = id;
    return true;
}

bool Phonebook::SetPhone(int id, std::string new_phone) {
    auto contact_iter = contact_map_.find(id);
    if (contact_iter == contact_map_.end()) {
        return false;
    }
    phone_map_.erase(contact_iter->second.GetNormPhone());
    contact_iter->second.phone_ = Phone(new_phone);
    phone_map_[contact_iter->second.GetNormPhone()] = id;
    return true;
}

bool Phonebook::SetAddress(int id, std::string new_address) {
    auto contact_iter = contact_map_.find(id);
    if (contact_iter == contact_map_.end()) {
        return false;
    }
    contact_iter->second.address_ = Address(new_address);
    return true;
}