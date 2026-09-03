#pragma once
#include "Component.h"

BEGIN(Engine)
// 오브젝트 하나에 대응하는 하나의 쉐이더를 부여한다.
class ENGINE_DLL Shader : public Component {
private:
	Shader(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
	Shader(CONST Shader& _PRTOBJ);
public:
	virtual ~Shader();

public:
	virtual HRESULT	Initialize_ProtoType(const _tchar* _FilePath, const D3D11_INPUT_ELEMENT_DESC* _DESC, uint32_t _ElemCount);
	virtual HRESULT	Initialize(VOID* _ARG);

public:
	HRESULT	Shader_Begin(uint32_t PassIndex = 0);
	HRESULT Bind_ShaderResourceView(const string& _ShaderVariableName, ComPtr<ID3D11ShaderResourceView>& _SRV);
	HRESULT UnBind_ShaderResourceView(const string& _ShaderVariableName);
	HRESULT Bind_ShaderResourceViewArray(const string& _ShaderVariableName, vector<ComPtr<ID3D11ShaderResourceView>> _SRVArray);

	HRESULT Bind_Matrix(const string& _ShaderVariableName, const XMFLOAT4X4* _Matrix);
	HRESULT Bind_MatrixArray(const string& _ShaderVariableName, const XMFLOAT4X4* _MatrixList, uint32_t _MatrixCount);
	HRESULT	Bind_RawValue(const string& _ShaderVariableName, const VOID* _Value, uint32_t _ByteLength);

	HRESULT Bind_TextureDirect(const string& _ShaderVariableName, filesystem::path _FilePath);

	ComPtr<ID3D11InputLayout>	Get_InputLayout(uint32_t _PassIndex) { return InputLayoutList[_PassIndex]; }

public:
	static	unique_ptr<Shader>		Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX,
										const _tchar* _FilePath, const D3D11_INPUT_ELEMENT_DESC* _DESC, uint32_t _ElemCount);
	virtual shared_ptr<Component>	Clone(VOID* _ARG);

private:
	ComPtr<ID3DX11Effect>				EffectObject;

	uint32_t							TechniquePassesCount;
	vector<ComPtr<ID3D11InputLayout>>	InputLayoutList;
};
END

// 정점 쉐이더 : 정점의 정보 받아 변환/연산을 하고 정점의 결과를 반환
// (Input 과 Output을 다르게 해서 정점의 구성을 바꿀 수 있다.(정점의 정보를 추가 or 간소화 할 수 있다.))
// -> 월드 변환 > 뷰 변환을 수행하고, 투영 행렬까지 행렬 곱한다. (투영 변환은 X)
// -> 뷰 변환의 필수적인 정보를 float4.w에 저장하기에 Output은 절대적으로 float4로 반환
// 
// 픽셀 쉐이더 : 정점 쉐이더의 연산을 마친 후에, 픽셀의 색을 최종적으로 결정해주는 쉐이더
// ("정점 쉐이더의 Output(픽셀 쉐이더 이전에 일어난 쉐이더 Output)이 픽셀 쉐이더 Input과 동일하다."(그래야 정보를 정상적으로 픽셀에 출력할 수 있게된다.))
// -> W 나누기(투영 변환), 뷰포트 변환, 레스터라이즈(픽셀 정보 생성)


// 픽셀 Output의 순서가 렌더 타겟의 순서와 같다. (렌더 타겟 3번 정점 출력 : Output 3번)
// 정점 쉐이더와 픽셀 쉐이더를 병렬적으로 연산(메인 함수)하고, 레스터라이즈 생성

// pass = 오브젝트의 쉐이더 기법을 달리하기 위해서 다른 함수를 (적용되었던 쉐이더 함수를 껐다 킬 수 있다. 쉐이더 세트 효과를 줄 수 있다.)
// EX : 쉐이더 3개가 적용되어있다면, 1번 쉐이딩만 적용시키고 싶다면 pass를 통해서 2번 3번을 끄고 1번을 사용한다.
// One pass 만 사용한다. 2 pass, 3 pass는 어쩔 수 없는 특수한 경우만 사용한다.

// input에 들어가는 정점으로는 로컬에 위치하고 있는 정점이다.
// 시멘틱 : 일
// 외부에서 가져온 값은 쉐이더 전역변수로 선언