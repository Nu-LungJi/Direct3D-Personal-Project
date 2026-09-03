#ifndef Engine_Macro_h__
#define Engine_Macro_h__

	#ifndef			MSG_BOX
	#define			MSG_BOX(_message)			MessageBox(NULL, TEXT(_message), L"System Message", MB_OK)
	#endif

	#define			BEGIN(NAMESPACE)		namespace NAMESPACE {
	#define			END						}
	
	#define			USING(NAMESPACE)	using namespace NAMESPACE;
	
	#ifdef	ENGINE_EXPORTS
	#define ENGINE_DLL		_declspec(dllexport)
	#else
	#define ENGINE_DLL		_declspec(dllimport)
	#endif

	#define COMPONENT_SINGLETON(CLASSNAME)	DECLARE_SINGLETON(CLASSNAME)
	#define STATE_SINGLETON(CLASSNAME)						\
			public:											\
			static CLASSNAME* GetInstance() {				\
				static CLASSNAME instance;					\
				return &instance;							\
			};
	#define NO_COPY(CLASSNAME)								\
			private:										\
			CLASSNAME(const CLASSNAME&) = delete;			\
			CLASSNAME& operator = (const CLASSNAME&) = delete;		
	
	#define DECLARE_SINGLETON(CLASSNAME)					\
			NO_COPY(CLASSNAME)								\
			public:											\
			static CLASSNAME&	GetInstance( void ){		\
				static CLASSNAME Instance;					\
				return Instance;							\
			}

	#define KEY_DOWN(KEY_IDX)				GameInstance::GetInstance().Get_InputManager()->KEY_STATE_DOWN(KEY_IDX)	== TRUE	// 해당 키 최초 입력
	#define KEY_HOLD(KEY_IDX)				GameInstance::GetInstance().Get_InputManager()->KEY_STATE_HOLD(KEY_IDX)	== TRUE	// 해당 키 입력 유지
	#define KEY_UP(KEY_IDX)					GameInstance::GetInstance().Get_InputManager()->KEY_STATE_UP(KEY_IDX)	== TRUE	// 해당 키 입력 중지
	
	#define MOUSE_LBUTTON					GameInstance::GetInstance().Get_InputManager()->MOUSE_LB_DOWN()					// 마우스 L 버튼 최초 입력
	#define MOUSE_RBUTTON					GameInstance::GetInstance().Get_InputManager()->MOUSE_RB_DOWN()					// 마우스 R 버튼 최초 입력

	#define MOUSE_LBUTTON_PRESSED			GameInstance::GetInstance().Get_InputManager()->MOUSE_LB_PRESSED()				// 마우스 L 버튼 지속 입력
	#define MOUSE_RBUTTON_PRESSED			GameInstance::GetInstance().Get_InputManager()->MOUSE_RB_PRESSED()				// 마우스 R 버튼 지속 입력

	#define MOUSE_MOVE(KEY_IDX)				GameInstance::GetInstance().Get_InputManager()->Get_MouseMove(KEY_IDX)
	#define MOUSE_WHEEL_FRONT				GameInstance::GetInstance().Get_InputManager()->Get_MouseMove(MOUSEMOVESTATE::MMS_W) > 0
	#define MOUSE_WHEEL_BACK				GameInstance::GetInstance().Get_InputManager()->Get_MouseMove(MOUSEMOVESTATE::MMS_W) < 0
	#define MOUSE_WHEEL_CLICK				GameInstance::GetInstance().Get_InputManager()->Get_MouseState(MOUSEKEYSTATE::MKS_MB) & 0x80

	#define MAKE_FLOAT_MATRIX_IDT		{	1.f, 0.f, 0.f, 0.f,				\
											0.f, 1.f, 0.f, 0.f,				\
											0.f, 0.f, 1.f, 0.f,				\
											0.f, 0.f, 0.f, 1.f				\
										};
	#define MAKE_FLOAT_MATRIX(XMMAT4X4)	{	XMMAT4X4.m[0][0], XMMAT4X4.m[0][1], XMMAT4X4.m[0][2], XMMAT4X4.m[0][3],	\
											XMMAT4X4.m[1][0], XMMAT4X4.m[1][1], XMMAT4X4.m[1][2], XMMAT4X4.m[1][3],	\
											XMMAT4X4.m[2][0], XMMAT4X4.m[2][1], XMMAT4X4.m[2][2], XMMAT4X4.m[2][3],	\
											XMMAT4X4.m[3][0], XMMAT4X4.m[3][1], XMMAT4X4.m[3][2], XMMAT4X4.m[3][3]	\
										};

#define DEFAULT_SCALING		 FLT_MAX 
#define RANDOM(MAX, MIN)	 (MIN + static_cast<_float>(rand()) / RAND_MAX * (MAX - MIN))

#endif // Engine_Macro_h__
