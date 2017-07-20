#include "database.h"

std::string errorToString(Database::Error r) {
	switch (r) {
		case Database::UNAUTHORIZED:
			return "UNAUTHORIZED";
		case Database::UNKOWN_ORDER:
			return "UNKOWN_ORDER";
		default:
			return "";
	}
}

typedef std::vector<Contract>::iterator Iterator;

Database::PostResult Database::post(const std::string & dealer, const std::string & side, const std::string & commodity, Amount amount, Price price) {
	db.push_back(Contract(last++, dealer, commodity, side, amount, price));
	return PostResult(db.back());
}

Database::RevokeResult Database::revoke(const std::string & dealer, ID id) {
	if (id >= db.size() || !db[id].amount || db[id].revoked)
		return RevokeResult(UNKOWN_ORDER);
	if (db[id].dealer == dealer) {
		db[id].revoked = true;
		return RevokeResult(id);
	}
	return RevokeResult(UNAUTHORIZED);
}

Database::CheckResult Database::check(const std::string & dealer, ID id) {
	if (id >= db.size())
		return CheckResult(UNKOWN_ORDER);
	if (db[id].dealer == dealer) {
		return db[id].revoked ? CheckResult(REVOKED) : (db[id].amount ? CheckResult(db[id]) : CheckResult(FILLED));
	}
	return CheckResult(UNAUTHORIZED);
}

Database::ListResult Database::list(const std::string&, const std::string & commodity, const std::string & dealerID) {
	std::vector<Contract> res;
	for (Iterator it = db.begin(); it != db.end(); ++it)
		if (!it->revoked && it->amount && (!commodity.size() || it->commodity == commodity) && (!dealerID.size() || it->dealer == dealerID))
			res.push_back(*it);
	return ListResult(res);
}

Database::AggressResult Database::aggress(ID id, Amount amount) {
	if (id >= db.size() || !db[id].amount || db[id].revoked)
		return AggressResult(UNKOWN_ORDER);
	if (amount > db[id].amount) {
		amount = db[id].amount;
		db[id].amount = 0;
	}
	else
		db[id].amount -= amount;
	Contract c(db[id]);
	c.amount = amount;
	c.changeSide();
	return AggressResult(c);
}
