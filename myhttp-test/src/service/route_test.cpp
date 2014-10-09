#include "gtest/gtest.h"
#include "route.hpp"
#include "request.hpp"

using namespace std;
using namespace myhttp::core;
using namespace myhttp::service;

TEST(RouteTest, Initialization) {
	const Route r("/user/:id/");
	EXPECT_EQ(r.getPath(), string("/user/:id/"));
}

TEST(RouteTest, PathParsingWithNoPlaceholders) {
	const Route r("/user/id/");
	auto parsedKeys = r.getPathKeys();
	vector<string> expectedKeys = { };
	EXPECT_EQ(expectedKeys, parsedKeys);
}

TEST(RouteTest, PathParsingWithPlaceholders) {
	const Route r("/user/:id/");
	auto parsedKeys = r.getPathKeys();
	vector<string> expectedKeys = { "id" };
	EXPECT_EQ(expectedKeys, parsedKeys);
}

TEST(RouteTest, MatchingWithNoParams) {
	const Route r("/user/id/");
	RouteParams rp;
	QueryParams qp;

	EXPECT_TRUE(r.isMatch("/user/id/", rp, qp));
	//TODO - paths should match regardless of the precense of a trailing forward slash
	//EXPECT_TRUE(r.isMatch("/user/id", rp, qp));

	EXPECT_FALSE(r.isMatch("/user/:id/", rp, qp));
	EXPECT_FALSE(r.isMatch("/USER/id/", rp, qp));
	EXPECT_FALSE(r.isMatch("/user/idd/", rp, qp));
	EXPECT_FALSE(r.isMatch("/students/user/id/", rp, qp));
}

TEST(RouteTest, MatchingWithRouteParams) {
	const Route r("/user/:id/");
	RouteParams rp;
	QueryParams qp;

	EXPECT_TRUE(r.isMatch("/user/A12345/", rp, qp));
	//TODO - paths should match regardless of the precense of a trailing forward slash
	//EXPECT_TRUE(r.isMatch("/user/A12345", rp, qp));

	EXPECT_FALSE(r.isMatch("/user/", rp, qp));
}

TEST(RouteTest, MatchingWithQueryParams) {
	const Route r("/user/");
	RouteParams rp;
	QueryParams qp;

	EXPECT_TRUE(r.isMatch("/user/?id=A12345", rp, qp));

	EXPECT_TRUE(r.isMatch("/user/?id=A12345&category=student", rp, qp));
	EXPECT_TRUE(r.isMatch("/user/??id=A12345", rp, qp));
	EXPECT_TRUE(r.isMatch("/user/?id", rp, qp));

	EXPECT_FALSE(r.isMatch("/students/user/?id=A12345", rp, qp));

	//TODO - test encoded characters

}

TEST(RouteTest, MatchingWithRouteAndQueryParams) {
	const Route r("/user/:id/");
	RouteParams rp;
	QueryParams qp;

	EXPECT_TRUE(r.isMatch("/user/A12345/?category=student", rp, qp));
	EXPECT_EQ("A12345", rp.getValue("id"));
	EXPECT_EQ("student", qp.getValue("category"));
}
