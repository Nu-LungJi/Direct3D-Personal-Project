#pragma once
#include "PCH.h"

namespace BehaviorTree {
	class BTBlackBoard {
	private:
		BTBlackBoard() = default;
	public:
		~BTBlackBoard() = default;

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
		static shared_ptr<BTBlackBoard> Create() {
			return shared_ptr<BTBlackBoard>(new BTBlackBoard());
		}
	private:
		unordered_map<string, std::any> DataBase;
	};

	class BTNode {
	protected:
		BTNode(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner) : BlackBoard(_BlackBoard), Owner(_Owner) {};
	public:
		virtual ~BTNode() = default;
		virtual NODESTATE Update(const _float _DT) = 0;
		virtual NODESTATE Get_NodeState() const { return NodeState; }

	protected:
		NODESTATE					NodeState;
		shared_ptr<BTBlackBoard>	BlackBoard;
		shared_ptr<GameObject>		Owner;
	};

	class Selector : public BTNode {
	public:
		Selector(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner) : BTNode(_BlackBoard, _Owner) {};
	public:
		VOID		AddNode(unique_ptr<BTNode> _ChildNode) { NodeList.emplace_back(move(_ChildNode)); }
		virtual NODESTATE	Update(const _float _DT) override {
			for (auto& Node : NodeList) {
				NODESTATE NodeState = Node->Update(_DT);
				if (NodeState != NODESTATE::FAILURE) return NodeState;
			}
			return NODESTATE::FAILURE;			// All Node return FAILURE
		}

	private:
		vector<unique_ptr<BTNode>>	NodeList;
	};

	class Sequencer : public BTNode {
	public:
		Sequencer(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner) : BTNode(_BlackBoard, _Owner) {};
	public:
		VOID		AddNode(unique_ptr<BTNode> _ChildNode) { NodeList.emplace_back(move(_ChildNode)); }
		virtual NODESTATE	Update(const _float _DT) override {
			for (auto& Node : NodeList) {
				NODESTATE NodeState = Node->Update(_DT);
				if (NodeState != NODESTATE::SUCCESS) return NodeState;
			}
			return NODESTATE::SUCCESS;			// All Node return SUCCESS
		}
	private:
		vector<unique_ptr<BTNode>>	NodeList;
	};

	class Paralle : public BTNode {
	public:
		Paralle(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner) : BTNode(_BlackBoard, _Owner) {};
	public:
		VOID	AddNode(unique_ptr<BTNode> _ChildNode) { NodeList.emplace_back(move(_ChildNode)); }
		virtual NODESTATE Update(const _float _DT) override {
			_bool AllSuccess = true;

			for (auto& Node : NodeList) {
				NODESTATE NodeState = Node->Update(_DT);
				if (NodeState == NODESTATE::FAILURE) { return NODESTATE::FAILURE; }
				if (NodeState == NODESTATE::RUNNING) { AllSuccess = false; }
			}

			if (AllSuccess) {
				return NODESTATE::SUCCESS;
			}
			return NODESTATE::RUNNING;
		}

	private:
		vector<unique_ptr<BTNode>>	NodeList;
	};

	class ConditionNode : public BTNode {
	public:
		ConditionNode(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner) : BTNode(_BlackBoard, _Owner) {};
	public:
		virtual NODESTATE Update(const _float _DT) = 0;
	};
	class ActionNode : public BTNode {
	public:
		ActionNode(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner) : BTNode(_BlackBoard, _Owner) {};
	public:
		virtual NODESTATE Update(const _float _DT) = 0;
	};

	class InverterNode : public BTNode {
	public:
		InverterNode(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner, unique_ptr<BTNode> _InvertedNode) 
			: BTNode(_BlackBoard, _Owner) , InvertedNode(move(_InvertedNode)){};
		virtual ~InverterNode() {};
	public:
		virtual NODESTATE Update(const _float _DT) override { 
			if (InvertedNode == nullptr) return NODESTATE::FAILURE;

			NODESTATE Result = InvertedNode->Update(_DT);
			if (Result == NODESTATE::SUCCESS) return NODESTATE::FAILURE;
			if (Result == NODESTATE::FAILURE) return NODESTATE::SUCCESS;

			return Result;		// RUNNING returns RUNNING
		}

	private:
		unique_ptr<BTNode>	InvertedNode = { nullptr };
	};

	class AlwaysSuccessNode : public ConditionNode {
	public:
		AlwaysSuccessNode(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner) : ConditionNode(_BlackBoard, _Owner) {};
		virtual ~AlwaysSuccessNode() {};

	public:
		virtual NODESTATE Update(const _float _DT) override { return NODESTATE::SUCCESS; }
	};
	class AlwaysFailureNode : public ConditionNode {
	public:
		AlwaysFailureNode(shared_ptr<BTBlackBoard> _BlackBoard, weak_ptr<GameObject> _Owner) : ConditionNode(_BlackBoard, _Owner) {};
		virtual ~AlwaysFailureNode() {};

	public:
		virtual NODESTATE Update(const _float _DT) override { return NODESTATE::FAILURE; }
	};
}