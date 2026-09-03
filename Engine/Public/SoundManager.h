#pragma once
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL SoundManager {
private:
	SoundManager(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);
public:
	~SoundManager();

public:
	HRESULT	Ready_SoundManager();
	VOID	Update_SoundManager();

	VOID	Release_SoundManager();

	HRESULT	Load_SoundBundle(filesystem::path _Directory, _bool _Looping);
	HRESULT Load_Sound(CONST wstring& _Key, CONST string& _FilePath, _bool _Looping);

	FMOD::Channel* Play_Sound(CONST wstring& _FileName, _bool _Looping);

	VOID	Stop_Channel(FMOD::Channel* _FChannel);

	VOID	Set_ChannelVolume(FMOD::Channel* _FChannel, CONST _float& _Volume);

	VOID	CrossFadeSound(FMOD::Channel* _CurrentChannel, CONST wstring& _NextSoundKey, _float _FadeDuration);

	FMOD::Channel*	FadeIn_Sound(CONST wstring& _Key, CONST _float& _Duration, _bool _Looping);
	VOID			FadeOut_Sound(FMOD::Channel* _FChannel, CONST _float& _Duration);
public:
	static	unique_ptr<SoundManager>	Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX);

private:
	ComPtr<ID3D11Device>			GRPDEV = { nullptr };
	ComPtr<ID3D11DeviceContext>		DEVCTX = { nullptr };

	unordered_map<wstring, FMOD::Sound*>	SoundList;

	FMOD::System* SoundSystem = { nullptr };
};

END