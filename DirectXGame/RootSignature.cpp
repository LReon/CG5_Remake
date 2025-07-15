#include "RootSignature.h"
#include "KamataEngine.h" // DirectXCommon

using namespace KamataEngine;

// RootSignatureを生成する
void RootSignature::Create() {
	// 既にインスタンスがあるなら開放する	// Createメンバ関数が　2度実行されたときの対処
	if (rootSignature_) {
		rootSignature_->Release();
		rootSignature_ = nullptr;
	}

	// クラス内で取得するために追加
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// RootSignature作成
	// 構造体にデータを用意する
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	// デスクリプタレンジ
	D3D12_DESCRIPTOR_RANGE srvDescRange[1]{};
	// t0 レジスタを利用可能にする
	srvDescRange[0].BaseShaderRegister = 0; // t0 レジスタを利用する
	srvDescRange[0].NumDescriptors = 1;     // 1つのSRVを利用する
	srvDescRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを利用する
	srvDescRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // 連続している

	D3D12_ROOT_PARAMETER rootParameters[1]{};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // デスクリプタテーブルを利用する
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;           // ピクセルシェーダーで利用する
	rootParameters[0].DescriptorTable.pDescriptorRanges = srvDescRange;           // デスクリプタレンジを設定
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(rootParameters); // デスクリプタレンジの数を設定

	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);

	D3D12_STATIC_SAMPLER_DESC staticSampler[1] = {};
	staticSampler[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // 線形フィルタを利用する
	staticSampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // U方向はラップアラウンド
	staticSampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // V方向はラップアラウンド
	staticSampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // W方向はラップアラウンド
	staticSampler[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較関数は常に真
	staticSampler[0].MaxLOD = D3D12_FLOAT32_MAX;                   // 最大LODは無限大
	staticSampler[0].ShaderRegister = 0;                           // s0 レジスタを利用する
	staticSampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // ピクセルシェーダーで利用する

	descriptionRootSignature.pStaticSamplers = staticSampler; // 静的サンプラーを設定
	descriptionRootSignature.NumStaticSamplers = _countof(staticSampler); // 静的サンプラーの数を設定

	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlog = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlog);
	if (FAILED(hr)) {
		DebugText::GetInstance()->ConsolePrintf(reinterpret_cast<char*>(errorBlog->GetBufferPointer()));
		assert(false);
	}

	// バイナリをもとに生成
	ID3D12RootSignature* rootSignature = nullptr;
	hr = dxCommon->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));

	// signatureBlob　は　RootSignatureの生成後解放してもいい
	signatureBlob->Release();

	// 生成した　RootSignature をとっておく
	rootSignature_ = rootSignature;
}

ID3D12RootSignature* RootSignature::Get() { return rootSignature_; }

// コンストラクタ
RootSignature::RootSignature() {}

// デストラクタ
RootSignature::~RootSignature() {
	if (rootSignature_) {
		rootSignature_->Release();
		rootSignature_ = nullptr;
	}
}