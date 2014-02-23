#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-CAR");
    BOOST_CHECK(GetBoolArg("-CAR"));
    BOOST_CHECK(GetBoolArg("-CAR", false));
    BOOST_CHECK(GetBoolArg("-CAR", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-CARo"));
    BOOST_CHECK(!GetBoolArg("-CARo", false));
    BOOST_CHECK(GetBoolArg("-CARo", true));

    ResetArgs("-CAR=0");
    BOOST_CHECK(!GetBoolArg("-CAR"));
    BOOST_CHECK(!GetBoolArg("-CAR", false));
    BOOST_CHECK(!GetBoolArg("-CAR", true));

    ResetArgs("-CAR=1");
    BOOST_CHECK(GetBoolArg("-CAR"));
    BOOST_CHECK(GetBoolArg("-CAR", false));
    BOOST_CHECK(GetBoolArg("-CAR", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noCAR");
    BOOST_CHECK(!GetBoolArg("-CAR"));
    BOOST_CHECK(!GetBoolArg("-CAR", false));
    BOOST_CHECK(!GetBoolArg("-CAR", true));

    ResetArgs("-noCAR=1");
    BOOST_CHECK(!GetBoolArg("-CAR"));
    BOOST_CHECK(!GetBoolArg("-CAR", false));
    BOOST_CHECK(!GetBoolArg("-CAR", true));

    ResetArgs("-CAR -noCAR");  // -CAR should win
    BOOST_CHECK(GetBoolArg("-CAR"));
    BOOST_CHECK(GetBoolArg("-CAR", false));
    BOOST_CHECK(GetBoolArg("-CAR", true));

    ResetArgs("-CAR=1 -noCAR=1");  // -CAR should win
    BOOST_CHECK(GetBoolArg("-CAR"));
    BOOST_CHECK(GetBoolArg("-CAR", false));
    BOOST_CHECK(GetBoolArg("-CAR", true));

    ResetArgs("-CAR=0 -noCAR=0");  // -CAR should win
    BOOST_CHECK(!GetBoolArg("-CAR"));
    BOOST_CHECK(!GetBoolArg("-CAR", false));
    BOOST_CHECK(!GetBoolArg("-CAR", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--CAR=1");
    BOOST_CHECK(GetBoolArg("-CAR"));
    BOOST_CHECK(GetBoolArg("-CAR", false));
    BOOST_CHECK(GetBoolArg("-CAR", true));

    ResetArgs("--noCAR=1");
    BOOST_CHECK(!GetBoolArg("-CAR"));
    BOOST_CHECK(!GetBoolArg("-CAR", false));
    BOOST_CHECK(!GetBoolArg("-CAR", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-CAR", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CAR", "eleven"), "eleven");

    ResetArgs("-CAR -bar");
    BOOST_CHECK_EQUAL(GetArg("-CAR", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CAR", "eleven"), "");

    ResetArgs("-CAR=");
    BOOST_CHECK_EQUAL(GetArg("-CAR", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CAR", "eleven"), "");

    ResetArgs("-CAR=11");
    BOOST_CHECK_EQUAL(GetArg("-CAR", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-CAR", "eleven"), "11");

    ResetArgs("-CAR=eleven");
    BOOST_CHECK_EQUAL(GetArg("-CAR", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-CAR", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-CAR", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-CAR", 0), 0);

    ResetArgs("-CAR -bar");
    BOOST_CHECK_EQUAL(GetArg("-CAR", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-CAR=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-CAR", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-CAR=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-CAR", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--CAR");
    BOOST_CHECK_EQUAL(GetBoolArg("-CAR"), true);

    ResetArgs("--CAR=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-CAR", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noCAR");
    BOOST_CHECK(!GetBoolArg("-CAR"));
    BOOST_CHECK(!GetBoolArg("-CAR", true));
    BOOST_CHECK(!GetBoolArg("-CAR", false));

    ResetArgs("-noCAR=1");
    BOOST_CHECK(!GetBoolArg("-CAR"));
    BOOST_CHECK(!GetBoolArg("-CAR", true));
    BOOST_CHECK(!GetBoolArg("-CAR", false));

    ResetArgs("-noCAR=0");
    BOOST_CHECK(GetBoolArg("-CAR"));
    BOOST_CHECK(GetBoolArg("-CAR", true));
    BOOST_CHECK(GetBoolArg("-CAR", false));

    ResetArgs("-CAR --noCAR");
    BOOST_CHECK(GetBoolArg("-CAR"));

    ResetArgs("-noCAR -CAR"); // CAR always wins:
    BOOST_CHECK(GetBoolArg("-CAR"));
}

BOOST_AUTO_TEST_SUITE_END()
