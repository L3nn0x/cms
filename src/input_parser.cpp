#include <map>
#include <vector>
#include <sstream>

#include "defines.h"
#include "database.h"
#include "result.h"

static bool isCommodity(const std::string &com) {
	for (size_t i = 0; i < sizeof(Commodity); ++i)
		if (Commodity[i] == com)
			return true;
	return false;
}

static bool isDealer(const std::string &dealer) {
	for (size_t i = 0; i < sizeof(DealerID); ++i)
		if (DealerID[i] == dealer)
			return true;
	return false;
}

static bool isSide(const std::string &side) {
	return side == "BUY" || side == "SELL";
}

template <typename T>
static inline bool parse(T &data, const std::string &s) {
	char c;
	std::stringstream ss(s);
	try {
		ss >> data;
	}
	catch (...) {
		return false;
	}
	if (ss.fail() || ss.get(c))
		return false;
	return true;
}

template <typename T>
static inline std::string to_string(const T& data) {
	std::stringstream ss;
	ss << data;
	return ss.str();
}

static std::vector<std::string> cutString(const std::string &s) {
	std::vector<std::string> res;
	size_t start = 0;
	size_t end = s.find(' ');
	while (end != std::string::npos) {
		res.push_back(s.substr(start, end - start));
		start = end + 1;
		end = s.find(' ', start);
	}
	res.push_back(s.substr(start));
	return res;
}

static std::string post(Database &db, const std::vector<std::string>& data) {
	if (data.size() != 6)
		return "INVALID_MESSAGE";
	if (!isSide(data[2]))
		return "INVALID_MESSAGE";
	if (!isCommodity(data[3]))
		return "INVALID_COMMODITY";
	size_t amount;
	float price;
	if (!parse(amount, data[4]))
		return "INVALID_MESSAGE";
	if (!parse(price, data[5]))
		return "INVALID_MESSAGE";
	Database::PostResult res = db.post(data[0], data[2], data[3], amount, price);
	if (!res)
		return errorToString(res.error);
	return to_string(res.result) + " HAS BEEN POSTED";
}

static std::string revoke(Database &db, const std::vector<std::string>& data) {
	if (data.size() != 3)
		return "INVALID_MESSAGE";
	ID id;
	if (!parse(id, data[2]))
		return "INVALID_MESSAGE";
	Database::RevokeResult res = db.revoke(data[0], id);
	if (!res)
		return errorToString(res.error);
	return to_string(res.result) + " HAS BEEN REVOKED";
}

static std::string check(Database &db, const std::vector<std::string>& data) {
	if (data.size() != 3)
		return "INVALID_MESSAGE";
	ID id;
	if (!parse(id, data[2]))
		return "INVALID_MESSAGE";
	Database::CheckResult res = db.check(data[0], id);
	if (!res) {
		if (res.error == Database::FILLED)
			return to_string(id) + " HAS BEEN FILLED";
		else if (res.error == Database::REVOKED)
			return to_string(id) + " HAS BEEN REVOKED";
		return errorToString(res.error);
	}
	return to_string(res.result);
}

static std::string list(Database &db, const std::vector<std::string>& data) {
	if (data.size() < 2 || data.size() > 4)
		return "INVALID_MESSAGE";
	std::string com;
	std::string dealer;
	if (data.size() > 2) {
		if (!isCommodity(data[2]))
			return "UNKOWN_COMMODITY";
		com = data[2];
	}
	if (data.size() > 3) {
		if (!isDealer(data[3]))
			return "UNKOWN_DEALER";
		dealer = data[3];
	}
	Database::ListResult res = db.list(data[0], com, dealer);
	if (!res)
		return errorToString(res.error);
	std::string output;
	for (std::vector<Contract>::const_iterator it = res.result.begin(); it != res.result.end(); ++it) {
		output += to_string(*it) + "\n";
	}
	return output + "END OF LIST";
}

static inline std::string makeTradeReport(Contract c) {
	std::stringstream ss;
	if (c.side == "BUY")
		ss << "BOUGHT";
	else
		ss << "SOLD";
	ss << " " << c.amount << " " << c.commodity << " @ " << c.price << " FROM " << c.dealer;
	return ss.str();
}

static std::string aggress(Database &db, const std::vector<std::string>& data) {
	if (data.size() < 4)
		return "INVALID_MESSAGE";
	std::string output;
	for (std::vector<std::string>::const_iterator it = data.begin() + 2; it != data.end(); it += 2) {
		ID id;
		Amount amount;
		if (!parse(id, *it))
			return "INVALID_MESSAGE";
		if (!parse(amount, *(it + 1)))
			return "INVALID_MESSAGE";
		Database::AggressResult res = db.aggress(id, amount);
		if (!res)
			return errorToString(res.error);
		output += makeTradeReport(res.result) + "\n";
	}
	return output;
}

typedef std::string(*Function)(Database&, const std::vector<std::string>&);

typedef std::map<std::string, Function> Functions;

static Functions initialize() {
	std::map<std::string, Function> res;
	res["POST"] = &post;
	res["REVOKE"] = &revoke;
	res["CHECK"] = &check;
	res["LIST"] = &list;
	res["AGGRESS"] = &aggress;
	return res;
}

const Functions functions = initialize();

std::string execute(Database &db, const std::string &input) {
	std::vector<std::string> data = cutString(input);
	if (data.size() < 2)
		return "INVALID_MESSAGE\n";
	if (!isDealer(data[0]))
		return "UNKOWN_DEALER\n";
	Functions::const_iterator it = functions.find(data[1]);
	if (it == functions.end())
		return "INVALID_MESSAGE\n";
	return it->second(db, data) + "\n";
}
