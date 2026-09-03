#pragma once

enum class NODESTATE { SUCCESS, FAILURE, RUNNING };

class BT {
private:
	BT();
	~BT();

public:
	HRESULT BehaviorTree_Initialize();
	VOID	BehaviorTree_Update(CONST FLOAT& _DT);

private:

};

class BTNode {
public:
	virtual ~BTNode();
	virtual NODESTATE Update();
};