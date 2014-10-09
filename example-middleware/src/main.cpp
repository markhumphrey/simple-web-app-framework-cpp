#include <vector>
#include <string>

#include "application.hpp"
#include "middleware_params.hpp"
#include "request.hpp"
#include "response.hpp"
#include "response_builder.hpp"
#include "server.hpp"

using namespace std;
using namespace myhttp::core;
using namespace myhttp::server;
using namespace myhttp::service;

// Example modeled off:
// https://github.com/strongloop/express/blob/master/examples/route-middleware/index.js

// Example requests:
//     curl http://localhost:3000/user/0
//     curl http://localhost:3000/user/0/edit
//     curl http://localhost:3000/user/1
//     curl http://localhost:3000/user/1/edit (unauthorized since this is not you)
//     curl -X DELETE http://localhost:3000/user/0 (unauthorized since you are not an admin)

namespace {

// "Database" functions
struct UserRecord {
	int id;
	string name;
	string email;
	string role;
};

// Normally this would fetch from a database
UserRecord fetchUserRecord(int id) {
	// dummy users
	static const vector<UserRecord> users = { { 0, "James", "james@example.com",
			"member" }, { 1, "John", "john@example.com", "member" }, { 2,
			"Mary", "mary@example.com", "admin" } };

	UserRecord record { -1, "", "", "" };
	if (id >= 0 && id < users.size()) {
		record = users[id];
	}

	return record;
}

// Helper functions to extract parameters from MiddlewarePArams
UserRecord getUserRecordParam(const string &key,
		const MiddlewareParams &mwParams) {
	UserRecord user { -1, "", "", "" };
	try {
		user = boost::any_cast<UserRecord>(mwParams.getValue(key));
	} catch (const boost::bad_any_cast &) {
	}
	return user;
}

UserRecord getAuthUserParam(const MiddlewareParams &mwParams) {
	return getUserRecordParam("authenticatedUser", mwParams);

}

UserRecord getUserParam(const MiddlewareParams &mwParams) {
	return getUserRecordParam("user", mwParams);
}

// Middleware
// middleware for fake authentication
bool loadAuthUser(const Request& req, Response& rep,
		MiddlewareParams& mwParams) {
	// always authentiate as id 0 for this fake authentication
	mwParams.setValue("authenticatedUser", fetchUserRecord(0));
	return true;
}

bool loadUser(const Request& req, Response& rep, MiddlewareParams& mwParams) {
	const string id = req.getParams().getValue("id");
	const UserRecord record = fetchUserRecord(atoi(id.c_str()));
	if (record.id < 0) {
		rep = ResponseBuilder().setStatus(Response::not_found).setContent(
				"Failed to load user " + id).build();
		return false;
	}
	mwParams.setValue("user", record);
	return true;
}

bool restrictUserToSelf(const Request& req, Response& rep,
		MiddlewareParams &mwParams) {
	const UserRecord user = getUserParam(mwParams);
	const UserRecord authUser = getAuthUserParam(mwParams);
	cout << "restrict user.id=" << user.id << " authUser.id=" << authUser.id
			<< endl;
	if (user.id != authUser.id) {
		rep = ResponseBuilder().setStatus(Response::unauthorized).setContent(
				"Unauthorized").build();
		return false;
	}
	return true;
}

bool restrictUserToRole(const Request& req, Response& rep,
		MiddlewareParams &mwParams, const string &role) {
	const UserRecord user = getUserParam(mwParams);
	if (user.role != role) {
		rep = ResponseBuilder().setStatus(Response::unauthorized).setContent(
				"Unauthorized").build();
		return false;
	}
	return true;
}

// Resources
// The verb handler definitions for resources
bool getUser(const Request& req, Response& rep, MiddlewareParams &mwParams) {
	const UserRecord user = getUserParam(mwParams);
	string content = "Viewing user " + user.name;
	rep = ResponseBuilder().setStatus(Response::ok).setContent(content).build();
	return true;
}
bool deleteUser(const Request& req, Response& rep, MiddlewareParams &mwParams) {
	const UserRecord user = getUserParam(mwParams);
	// for now, don't actually delete the record
	string content = "Deleted user " + user.name;
	rep = ResponseBuilder().setStatus(Response::ok).setContent(content).build();
	return true;
}

bool getUserEdit(const Request& req, Response& rep,
		MiddlewareParams &mwParams) {
	const UserRecord user = getUserParam(mwParams);
	string content = "Editing user " + user.name;
	rep = ResponseBuilder().setStatus(Response::ok).setContent(content).build();
	return true;
}

}

int main() {
	// create application with desired http server implementation
	shared_ptr<ServerInterface> server(new Server());
	Application app(server);

	using namespace std::placeholders;
	// add middleware
	app.addMiddleware(bind(loadAuthUser, _1, _2, _3));

	// add resources
	{
		Resource res(Route("/user/:id"));
		res.addAll(bind(loadUser, _1, _2, _3));
		res.addGet(bind(getUser, _1, _2, _3));
		res.addDelete(bind(restrictUserToRole, _1, _2, _3, "admin"));
		res.addDelete(bind(deleteUser, _1, _2, _3));
		app.addResource(res);
	}

	{
		Resource res(Route("/user/:id/edit"));
		res.addAll(bind(loadUser, _1, _2, _3));
		res.addGet(bind(restrictUserToSelf, _1, _2, _3));
		res.addGet(bind(getUserEdit, _1, _2, _3));
		app.addResource(res);
	}

	// start server
	const int port = 3000;
	app.listen(port);
	return EXIT_SUCCESS;
}

