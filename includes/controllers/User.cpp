#include "User.h"
#include <cppcms/service.h>  
#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>
#include <cppcms/http_response.h>
#include <cppdb/frontend.h>
#include <picojson.h>

// constructor
User::User(cppcms::service &srv) : Master(srv)
{
    dispatcher().assign("", &User::getUser, this);
    mapper().assign("");

    dispatcher().assign("/(\\d+)", &User::getUserById, this, 1);
    mapper().assign("/{1}");
}

void User::getUser()
{
    try {
        cppdb::result res = sql() << "SELECT * FROM users";

        picojson::array usersArray;

        while (res.next()) {
            picojson::object userJson;
            std::string id, firstName, lastName, loginId;
            
            res.fetch(0, id);
            res.fetch(1, firstName);
            res.fetch(2, lastName);
            res.fetch(3, loginId);

            int userId = std::stoi(id);
            userJson["id"] = picojson::value(static_cast<double>(userId));
            userJson["firstName"] = picojson::value(firstName);
            userJson["lastName"] = picojson::value(lastName);
            userJson["email"] = picojson::value(loginId);

            usersArray.push_back(picojson::value(userJson));
        }
        if (usersArray.empty()) {
            response().status(404);
            response().set_content_header("application/json");
            response().out() << R"({"error": "User not found"})";
        }
        else{
            picojson::value jsonResponse(usersArray);
            response().content_type("application/json");
            response().out() << jsonResponse.serialize();
        }
    }
    catch (std::exception &e) {
        picojson::object errorJson;
        errorJson["error"] = picojson::value(e.what());

        picojson::value jsonResponse(errorJson);
        response().status(500);
        response().content_type("application/json");
        response().out() << jsonResponse.serialize();
    }
}

void User::getUserById(std::string id)
{
    try {
        cppdb::result res = sql() << "SELECT * FROM users WHERE usrsId = ?" << id;

        picojson::object userJson;

        while (res.next()) {
            std::string firstName, lastName, loginId;
            
            res.fetch(0, id);
            res.fetch(1, firstName);
            res.fetch(2, lastName);
            res.fetch(3, loginId);
            
            int userId = std::stoi(id);
            userJson["id"] = picojson::value(static_cast<double>(userId));
            userJson["firstName"] = picojson::value(firstName);
            userJson["lastName"] = picojson::value(lastName);
            userJson["loginId"] = picojson::value(loginId);
        }
        if (userJson.empty()) {
            response().status(404);
            response().set_content_header("application/json");
            response().out() << R"({"error": "User not found"})";
        }
        else{
            picojson::value jsonResponse(userJson);
            response().content_type("application/json");
            response().out() << jsonResponse.serialize();
        }
    }
    catch (std::exception &e) {
        picojson::object errorJson;
        errorJson["error"] = picojson::value(e.what());

        picojson::value jsonResponse(errorJson);
        response().status(500);
        response().content_type("application/json");
        response().out() << jsonResponse.serialize();
    }
}
