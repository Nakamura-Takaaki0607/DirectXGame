#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DebugText.h"


/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	// ビュープロジェクション (共通)
	ViewProjection viewProjection_;

	// ステージ
	uint32_t textureHandleStage_ = 0;
	Model* modelStage_ = 0;
	WorldTransform worldTransformStage_[20];

	// プレイヤー
	uint32_t textureHandlePlayer_ = 0;
	Model* modelPlayer_ = nullptr;
	WorldTransform worldTransformPlayer_;

	// ビーム
	uint32_t textureHandleBeam_ = 0;
	Model* modelBeam_ = 0;
	WorldTransform worldTransformBeam_[100];

	// 敵
	uint32_t textureHandleEnemy_ = 0;
	Model* modelEnemy_ = 0;
	WorldTransform worldTransformEnemy_[100];

	// スコアアップ
	uint32_t textureHandleScoreUp_ = 0;
	Model* modelScoreUp_ = 0;
	WorldTransform worldTransformScoreUp_[100];

	// タイトル（スプライト）
	uint32_t textureHandleTitle_ = 0;
	Sprite* spriteTitle_ = nullptr;

	// ゲームオーバー（スプライト）
	uint32_t textureHandleGameOver_ = 0;
	Sprite* spriteGameOver_ = nullptr;

	// エンター表示（スプライト）
	uint32_t textureHandleEnter_ = 0;
	Sprite* spriteEnter_ = nullptr;

	// スコア数値（スプライト）
	uint32_t textureHandleNumber_ = 0;
	Sprite* spriteNumber_[5] = {};

	// ライフ残機（2Dスプライト）
	uint32_t textureHandleLife_ = 0;
	Sprite* spriteLife_[3] = {};

	// スコア表示
	uint32_t textureHandleScore_ = 0;
	Sprite* spriteScore_ = nullptr;

	// ステージ更新
	void StageUpdate();

	// スコア数値更新
	void DrawScore();

	void DrawScoreUp();

	// プレイヤー更新
	void PlayerUpdate();

	// ビーム更新
	void BeamUpdate();

	// ビーム移動
	void BeamMove();

	// ビーム発生
	void BeamBorn();

	int beamFlag_[100] = {}; // ビーム存在フラグ（0:存在しない、1:存在する）

	// 敵更新
	void EnemyUpdate();

	// 敵移動
	void EnemyMove();

	// 敵発生
	void EnemyBorn();

	int enemyFlag_[100] = {}; // 敵存在フラグ（0:存在しない、1:存在する）

	// 敵のスピード
	float enemySpeed_[100] = {};

	// 敵ジャンプの移動速度
	float enemyJumpSpeed_[100] = {};

	// 敵ジャンプ
	void EnemyJump();

	// 衝突判定
	void Collision();

	// 衝突判定（プレイヤーと敵）
	void CollisionPlayerEnemy();

	// 衝突判定（プレイヤーとスコアアップ）
	void CollisionPlayerScoreUp();

	// 衝突判定（ビームと敵）
	void CollisionBeamEnemy();

	// 衝突判定（ビームとスコアアップ）
	void CollisionBeamScoreUp();

	// スコアアップ更新
	void ScoreUpUpdate();

	// スコアアップ移動
	void ScoreUpMove();

	// スコアアップ発生
	void ScoreUpBorn();

	// スコアアップジャンプ
	void ScoreUpJump();

	// ゲームスコア
	int gameScore_ = 0;

	// スコアアップフラグ
	int scoreUpFlag_[100] = {};

	// スコアアップのスピード
	float scoreUpSpeed_[100] = {};

	// スコアアップのジャンプスピード
	float scoreUpJumpSpeed_[100] = {};

	// プレイヤーライフ
	int playerLife_ = 3;

	// プレイヤータイマ
	int playerTimer_ = 0;

	// オリジナル追加プログラム用
	int flag = 0;

	// ゲームプレイ更新
	void GamePlayUpdate();

	// ゲームプレイ更新3D描画
	void GamePlayDraw3D();

	// ゲームプレイ2D背景描画
	void GamePlayDraw2DBack();

	// ゲームプレイ2D近景描画
	void GamePlayDraw2DNear();

	// シーンモード（0:ゲームプレイ　1:タイトル）
	int sceneMode_ = 1;

	// タイトル更新
	void TitleUpdate();

	// タイトル2D
	void TitleDraw2DNear();

	// ゲームオーバー更新
	void GameOverUpdate();

	// ゲームオーバー2D
	void GameOverDraw2DNear();

	// ゲームタイマー
	int gameTimer_ = 0;

	// 初期化
	void GamePlayStart();

	// ビーム発射タイマー
	int beamTimer_ = 0;

	// サウンド
	uint32_t soundDataHandleTitleBGM_ = 0;    // タイトルBGM
	uint32_t soundDataHandleGamePlayBGM_ = 0; // ゲームプレイBGM
	uint32_t soundDataHandleGameOverBGM_ = 0; // ゲームオーバーBGM
	uint32_t soundDataHandleEnemyHitSE_ = 0;  // 敵ヒットSE
	uint32_t soundDataHandlePlayerHitSE_ = 0; // プレイヤーヒットSE
	uint32_t voiceHandleBGM_ = 0;             // 音声再生ハンドル

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	// BG (スプライト)
	uint32_t textureHandleBG_ = 0;
	Sprite* spriteBG_ = nullptr;

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	DebugText* debugText_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
