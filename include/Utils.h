#pragma once
class Utils
{
public:
	template <typename T>
	static constexpr T absDiff(T a, T b) {
		return (a > b) ? (a - b) : (b - a);
	}
};

