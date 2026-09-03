#include "SoundManager.h"

SoundManager::SoundManager(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) : GRPDEV(_GRPDEV), DEVCTX(_DEVCTX) {}
SoundManager::~SoundManager() { Release_SoundManager(); }

HRESULT SoundManager::Ready_SoundManager() {
	if (FMOD::System_Create(&SoundSystem)) return E_FAIL;

	if (SoundSystem->init(32, FMOD_INIT_NORMAL, nullptr) != FMOD_OK) return E_FAIL;

	return S_OK;
}
VOID	SoundManager::Update_SoundManager() {
	if (nullptr != SoundSystem) 
		SoundSystem->update();
}
VOID	SoundManager::Release_SoundManager(){

	for (auto& pair : SoundList) {
		if (pair.second) pair.second->release();
	}
	SoundList.clear();
	
	if (SoundSystem) {
		SoundSystem->close();
		SoundSystem->release();
		SoundSystem = nullptr;
	}
}

HRESULT SoundManager::Load_SoundBundle(filesystem::path _Directory, _bool _Looping){
	if (!filesystem::exists(_Directory) || !filesystem::is_directory(_Directory))		return E_FAIL;
	
	for (auto& SoundFile : filesystem::directory_iterator(_Directory)) {
		if (filesystem::is_regular_file(SoundFile)) {
			filesystem::path FilePath = SoundFile.path();
			string Extension = FilePath.extension().string();

			if (Extension == ".mp3" || Extension == ".wav" || Extension == ".ogg") {
				if (FAILED(Load_Sound(FilePath.stem().wstring(), FilePath.string(), _Looping))) {
					wstring Message = L"Cannot Load SoundFile : " + FilePath.stem().wstring();
					MessageBox(NULL, Message.c_str(), L"System Message", MB_OK);
				}
			}
		}
	}
}

HRESULT SoundManager::Load_Sound(const wstring& _Key, const string& _FilePath, _bool _Looping){
	if (SoundList.find(_Key) != SoundList.end()) return S_OK;

	FMOD::Sound* SoundObject = nullptr;

	if (SoundSystem->createSound(_FilePath.c_str(), (_Looping) ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF, nullptr, &SoundObject) == FMOD_OK) {
		SoundList[_Key] = SoundObject;
		return S_OK;
	}

	return E_FAIL;
}

FMOD::Channel* SoundManager::Play_Sound(const wstring& _FileName, _bool _Looping){
	auto iter = SoundList.find(_FileName);
	if (iter == SoundList.end()) return nullptr;

	FMOD::Sound* FSound = iter->second;
	FSound->setMode(_Looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);

	FMOD::Channel* FChannel = nullptr;
	SoundSystem->playSound(FSound, nullptr, false, &FChannel);

	return FChannel;
}

VOID SoundManager::Stop_Channel(FMOD::Channel* _FChannel) {
	if (!_FChannel) return;
	bool isPlaying = false;

	_FChannel->isPlaying(&isPlaying);

	if (isPlaying) 
		_FChannel->stop();
}
VOID SoundManager::Set_ChannelVolume(FMOD::Channel* _FChannel, const _float& _Volume) {
	if (!_FChannel) return;
	_FChannel->setVolume(_Volume);
}

VOID SoundManager::CrossFadeSound(FMOD::Channel* _CurrentChannel, const wstring& _NextSoundKey, _float _FadeDuration) {
	if (_CurrentChannel) {
		FadeOut_Sound(_CurrentChannel, _FadeDuration);
	}
	FMOD::Channel* NextChannel = FadeIn_Sound(_NextSoundKey, _FadeDuration, true);
}

FMOD::Channel* SoundManager::FadeIn_Sound(CONST wstring& _Key, CONST _float& _Duration, _bool _Looping) {
	auto it = SoundList.find(_Key);
	if (it == SoundList.end()) return nullptr;

	FMOD::Sound* sound = it->second;
	sound->setMode(_Looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);

	FMOD::Channel* channel = nullptr;
	SoundSystem->playSound(sound, nullptr, true, &channel);
	if (!channel) return nullptr;

	unsigned long long parentClock = 0;
	int rate = 0;
	SoundSystem->getSoftwareFormat(&rate, nullptr, nullptr);
	channel->getDSPClock(nullptr, &parentClock);

	channel->setVolume(0.0f);

	unsigned long long fadeEndClock = parentClock + (rate * _Duration);
	channel->addFadePoint(parentClock, 0.0f);
	channel->addFadePoint(fadeEndClock, 1.0f);

	channel->setPaused(false);

	return channel;
}
VOID SoundManager::FadeOut_Sound(FMOD::Channel* _FChannel, CONST _float& _Duration) {
	if (!_FChannel) return;

	unsigned long long parentClock;
	int rate;
	SoundSystem->getSoftwareFormat(&rate, nullptr, nullptr);
	_FChannel->getDSPClock(nullptr, &parentClock);

	float currentVolume;
	_FChannel->getVolume(&currentVolume);
	_FChannel->addFadePoint(parentClock, currentVolume);
	_FChannel->addFadePoint(parentClock + (rate * _Duration), 0.0f);

	_FChannel->setDelay(0, parentClock + (rate * _Duration), true);
}
unique_ptr<SoundManager>	SoundManager::Create(ComPtr<ID3D11Device> _GRPDEV, ComPtr<ID3D11DeviceContext> _DEVCTX) {
	auto Instance = unique_ptr<SoundManager>(new SoundManager(_GRPDEV, _DEVCTX));
	if (FAILED(Instance->Ready_SoundManager()))
		MSG_BOX("Cannot Create SoundManager.");

	return Instance;
}
