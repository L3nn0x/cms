#pragma once

#include <vector>
#include <utility>
#include <ostream>

#include "defines.h"
#include "result.h"

struct Contract {
	ID id;
	std::string dealer;
	std::string commodity;
	std::string side;

	Amount amount;
	Price price;
	bool revoked;

	Contract() : amount(0), revoked(true) {}
	Contract(ID id, const std::string &dealer, const std::string &com, const std::string &side, Amount amount, Price price)
		: id(id), dealer(dealer), commodity(com), side(side), amount(amount), price(price), revoked(false) {}

	friend std::ostream& operator<<(std::ostream& os, const Contract &c) {
		os << c.id << " " << c.dealer << " " << c.side << " " << c.commodity << " " << c.amount << " " << c.price;
		return os;
	}

	void changeSide() {
		if (side == "BUY") side = "SELL";
		else if (side == "SELL") side = "BUY";
	}
};

class Database {
public:
	enum Error {
		OK,
		FILLED,
		REVOKED,
		UNAUTHORIZED,
		UNKOWN_ORDER
	};

	typedef Result<Error, Contract> PostResult;
	typedef Result<Error, ID> RevokeResult;
	typedef Result<Error, Contract> CheckResult;
	typedef Result<Error, std::vector<Contract> > ListResult;
	typedef Result<Error, Contract> AggressResult;

	Database() : last(0) {}

	PostResult post(const std::string &dealer, const std::string &side, const std::string &commodity, Amount amount, Price price);
	RevokeResult revoke(const std::string &dealer, ID id);
	CheckResult check(const std::string &dealer, ID id);
	ListResult list(const std::string &dealer, const std::string &commodity = "", const std::string &dealerID = "");
	AggressResult aggress(ID id, Amount amount);

private:
	ID last;
	std::vector<Contract> db;
};

std::string errorToString(Database::Error);
