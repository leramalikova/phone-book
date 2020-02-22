#include <catch.hpp>
#include <ctime>
#include <phonebook.h>

TEST_CASE("Test Normalised") {
    REQUIRE(NormalizePhone("100500") == "100500");
    REQUIRE(NormalizePhone("+72345") == "72345");
    REQUIRE(NormalizeName("Jane Doe") == "doe jane");
    REQUIRE(NormalizeName("A c B a") == "a a b c");
    REQUIRE(NormalizeName("JoHn   SnOW  ") == "john snow");
    REQUIRE(NormalizeName("  aa   bbb  ccc    ") == "aa bbb ccc");
}

TEST_CASE("Test Add & Find") {
    Phonebook book;
    REQUIRE(book.AddContact("Harry Potter", "123", "4 Privet Drive, Little Whinging, Surrey") == 0);
    REQUIRE(book.AddContact("Ron Weasley", "+456", "The Burrow") == 1);
    REQUIRE(book.AddContact("Ron Weasley", "9999") == -1);
    REQUIRE(book.AddContact("abbaa", "123") == -1);

    REQUIRE(book.FindByName(" harry potter ") == 0);
    REQUIRE(book.FindByName(" rOn wEaSley ") == 1);
    REQUIRE(book.FindByName("Hermiony Granger") == -1);

    Contact contact(2, "albus percival dumbledore", "+0000", "hogwarts");
    REQUIRE(book.AddContact("albus dumbledore percival", "0000", "hogwarts") == 2);
    REQUIRE(book.Valid(contact));

    REQUIRE(book.FindByPhone("+123") == 0);
    REQUIRE(book.FindByPhone("+456") == 1);
    REQUIRE(book.FindByPhone("0000") == 2);
}

TEST_CASE("Test Get") {
    Phonebook book;
    book.AddContact("aba cba", "111");
    book.AddContact("bhb rkd", "234", "some place");
    REQUIRE(book.GetAddress(0) == std::nullopt);
    REQUIRE(book.GetAddress(1) == "some place");

    book.AddContact("Potter Harry", "+123", "4 Privet Drive, Little Whinging, Surrey");
    REQUIRE(book.GetName(0) == "aba cba");
    REQUIRE(book.GetName(1) == "bhb rkd");
    REQUIRE(book.GetName(2) == "Potter Harry");

    REQUIRE(book.GetPhone(0) == "111");
    REQUIRE(book.GetPhone(1) == "234");
    REQUIRE(book.GetPhone(2) == "+123");
}

TEST_CASE("Test Filter") {
    Phonebook book;
    book.AddContact("aba", "12", "yellow street 6");
    book.AddContact("bab", "23", "green street 5");
    book.AddContact("cba", "34", "black house");

    auto filtered = book.FilterByAddress("street");
    REQUIRE(filtered.size() == 2);

    Contact contact1 = Contact(0, "aba", "12", "yellow street 6");
    Contact contact2 = Contact(1, "bab", "23", "green street 5");
    Contact contact3 = Contact(1, "cba", "34", "black house");

    REQUIRE(std::find(filtered.begin(), filtered.end(), contact1) != filtered.end());
    REQUIRE(std::find(filtered.begin(), filtered.end(), contact2) != filtered.end());
    REQUIRE(std::find(filtered.begin(), filtered.end(), contact3) == filtered.end());
}

TEST_CASE("Test Erase") {
    Phonebook book;
    book.AddContact("zzzz", "32", "house");
    book.AddContact("woohoo", "64");

    REQUIRE(book.FindByName("zzzz") == 0);
    REQUIRE(book.FindByPhone("32") == 0);
    REQUIRE(book.FilterByAddress("house").size() == 1);

    book.EraseByID(0);
    REQUIRE(book.FindByName("zzzz") == -1);
    REQUIRE(book.FindByPhone("32") == -1);
    REQUIRE(book.FilterByAddress("house").empty());
}

TEST_CASE("Test Set") {
    Phonebook book;
    book.AddContact("anna A", "8923456");
    book.AddContact("bob B", "3456");
    book.AddContact("clare c", "0001");

    REQUIRE(!book.SetAddress(4, "234"));
    REQUIRE(book.SetAddress(0, "house"));
    REQUIRE(book.GetAddress(0) == "house");

    REQUIRE(book.SetName(1, "billy bob b"));
    REQUIRE(book.GetName(1) == "billy bob b");

    REQUIRE(book.SetPhone(2, "+2"));
    REQUIRE(book.GetPhone(2) == "+2");

    REQUIRE(book.AddContact("jane", "+2") == -1);
    REQUIRE(book.AddContact("Bob Billy B", "333") == -1);
    REQUIRE(book.AddContact("milly", "1234567", "house") == 3);
}

double AverageFindByName(int n) {
    Phonebook book;
    for (int i = 0; i < n; i++) {
        book.AddContact("a" + std::to_string(i), std::to_string(i));
    }
    double begin = clock();
    for (int i = 0; i < n; i++) {
        book.FindByName("a" + std::to_string(i));
    }
    double end = clock();
    return (end - begin) / n;
}

double AverageFindByPhone(int n) {
    Phonebook book;
    for (int i = 0; i < n; i++) {
        book.AddContact("a" + std::to_string(i), std::to_string(i));
    }
    double begin = clock();
    for (int i = 0; i < n; i++) {
        book.FindByPhone(std::to_string(i));
    }
    double end = clock();
    return (end - begin) / n;
}

double MaxAverageByName(int n_max, int step) {
    // максимальное среднее время работы для n <= n_max
    double max_name = 0;
    for (int n = 1; n < n_max; n += step) {
        max_name = std::max(max_name, AverageFindByName(n));
    }
    return max_name;
}

double MaxAverageByPhone(int n_max, int step) {
    // максимальное среднее время работы для n <= n_max
    double max_phone = 0;
    for (int n = 1; n < n_max; n += step) {
        max_phone = std::max(max_phone, AverageFindByPhone(n));
    }
    return max_phone;
}

TEST_CASE("Test time") {
    // должны не сильно увеличиваться?
    REQUIRE((MaxAverageByName(1000, 50) / MaxAverageByName(50, 1)) < 2);
    REQUIRE((MaxAverageByPhone(1000, 50) / MaxAverageByPhone(50, 1)) < 2);
    REQUIRE((MaxAverageByName(10000, 500) / MaxAverageByName(50, 1)) < 2);
    REQUIRE((MaxAverageByPhone(10000, 500) / MaxAverageByPhone(50, 1)) < 2);
}
