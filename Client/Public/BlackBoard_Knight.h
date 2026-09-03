#pragma once

using namespace BehaviorTree;

class BlackBoard_Knight {
private:
	BlackBoard_Knight() = default;
public:
	~BlackBoard_Knight() = default;

public:
	template<typename T>
	void	Set_Value(const string& _Key, const T& _Value) { DataBase[_Key] = _Value; }

	template<typename T>
	T		Get_Value(const string& _Key) {
		auto Data = DataBase.find(_Key);
		if (Data == DataBase.end()) return T();
		return any_cast<T>(DataBase[_Key]);
	}

	_bool	Has_Key(const string& _Key) {
		return DataBase.find(_Key) != DataBase.end();
	}

public:
	static shared_ptr<BlackBoard_Knight> Create();

private:
	unordered_map<string, any> DataBase;
};

