#pragma once

template <typename T, typename U = bool>
struct Result {
	bool isError;
	T error;
	U result;

	Result(T error) : isError(true), error(error) {}
	Result(U result) : isError(false), result(result) {}

	bool operator!() const { return isError; }
};