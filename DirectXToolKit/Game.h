#pragma once
#include <vector>
#include <fstream>
#include <sstream>
#include "DeviceResources.h"
#include "StepTimer.h"


#pragma warning(push)
#pragma warning(disable:26812)
#pragma warning(disable:26451)
#pragma warning(disable:26495)
#pragma warning(disable:6319)
#pragma warning(disable:6386)
#pragma warning(disable:6385)
#include "rapidjson/document.h"
#pragma warning(pop)

class Game final : public DX::IDeviceNotify // 더이상 Game에서 자식을 못받음
{
private:
	std::unique_ptr<DX::DeviceResources>		m_deviceResources;

	DX::StepTimer								m_timer;

	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texCat;
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texBug;

	std::unique_ptr<DirectX::SpriteBatch>			 m_spriteBatch;
	std::unique_ptr<DirectX::CommonStates>			 m_commonStates;

	std::unique_ptr<DirectX::Keyboard>			m_keyBoard;
	std::unique_ptr<DirectX::Mouse>				m_mouse;

	int m_currentFrame{ 0 };
	double m_timeToNextFrame{ 0.1f };

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

	std::vector<RECT> m_rects;
public:
	Game() noexcept(false);
	~Game();

	Game(Game&&) = default; // 이동 연산자
	Game& operator=(Game&&) = default;	// 이동 대입 

	Game(Game const&) = delete;
	Game& operator=(Game const&) = delete; // 복사 이동

	void Initialize(HWND window, int width, int height);

	void Tick();

	void OnDeviceLost() override;
	void OnDeviceRestored() override;

	void OnActivated();
	void OnDeActivated();
	void OnSuspending();
	void OnResuming();
	void OnWindowMoved();
	void OnWindowSizeChange(int width, int height);

	void GetDefaultSize(int& width, int& height) const noexcept;

private:
	void LoadSpriteSheetFromJSON();

	void Update(DX::StepTimer const& timer);
	void Render();

	void Clear();

	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();
};

