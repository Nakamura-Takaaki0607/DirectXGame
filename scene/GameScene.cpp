#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "MathUtilityForText.h"
#include <time.h>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete spriteBG_;   // BG
	delete modelStage_; // ステージ

	delete modelPlayer_; // プレイヤー
	delete modelBeam_;   // ビーム

	delete spriteTitle_;    // タイトル
	delete spriteGameOver_; // ゲームオーバー
	delete spriteEnter_;    // エンター表示
}

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// デッバクテキスト
	debugText_ = DebugText::GetInstance();
	debugText_->Initialize();

	srand((unsigned int)time(NULL));

	// BG(2Dスプライト)
	textureHandleBG_ = TextureManager::Load("bg.png");
	spriteBG_ = Sprite::Create(textureHandleBG_, {0, 0});

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	// ステージ
	textureHandleStage_ = TextureManager::Load("stage2.jpg");
	modelStage_ = Model::Create();
	// ステージの数ループする
	for (int i = 0; i < 20; i++) {
		worldTransformStage_[i].Initialize();
	}

	// プレイヤー
	textureHandlePlayer_ = TextureManager::Load("player.png");
	modelPlayer_ = Model::Create();
	worldTransformPlayer_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformPlayer_.Initialize();

	// ビーム
	textureHandleBeam_ = TextureManager::Load("beam.png");
	modelBeam_ = Model::Create();
	for (int i = 0; i < 10; i++) {
		worldTransformBeam_[i].scale_ = {0.5f, 0.5f, 0.5f};
		worldTransformBeam_[i].Initialize();
	}

	// 敵
	textureHandleEnemy_ = TextureManager::Load("Enemy.png");
	modelEnemy_ = Model::Create();
	// 敵の数ループする
	for (int i = 0; i < 10; i++) {
		worldTransformEnemy_[i].scale_ = {0.5f, 0.5f, 0.5f};
		worldTransformEnemy_[i].Initialize();
	}

	// スコアアップ
	textureHandleScoreUp_ = TextureManager::Load("ScoreUp.png");
	modelScoreUp_ = Model::Create();
	// ループ処理
	for (int i = 0; i < 10; i++) {
		worldTransformScoreUp_[i].scale_ = {0.5f, 0.5f, 0.5f};
		worldTransformScoreUp_[i].Initialize();
	}

	// タイトル（2Dスプライト）
	textureHandleTitle_ = TextureManager::Load("title.png");
	spriteTitle_ = Sprite::Create(textureHandleTitle_, {0, 0});

	// ゲームオーバー（2Dスプライト）
	textureHandleGameOver_ = TextureManager::Load("gameover.png");
	spriteGameOver_ = Sprite::Create(textureHandleGameOver_, {0, 0});

	// エンター表示（2Dスプライト）
	textureHandleEnter_ = TextureManager::Load("enter.png");
	spriteEnter_ = Sprite::Create(textureHandleEnter_, {400, 450});

	// スコア数値(2Dスプライト)
	textureHandleNumber_ = TextureManager::Load("number.png");
	for (int i = 0; i < 5; i++) {
		spriteNumber_[i] = Sprite::Create(textureHandleNumber_, {300.0f + i * 26, 0});
	}

	// スコア表示
	textureHandleScore_ = TextureManager::Load("score.png");
	spriteScore_ = Sprite::Create(textureHandleScore_, {170, 0});

	// ライフ(2Dスプライト)
	for (int i = 0; i < 3; i++) {
		spriteLife_[i] = Sprite::Create(textureHandlePlayer_, {800.0f + i * 60, 10});
		spriteLife_[i]->SetSize({40, 40});
	}

	// ビュープロジェクションの初期化
	viewProjection_.translation_.y = 1;
	viewProjection_.translation_.z = -6;
	viewProjection_.Initialize();

	// ステージの位置を変更
	for (int i = 0; i < 20; i++) {
		worldTransformStage_[i].translation_ = {0, -1.5f, 2.0f * i - 5};
		worldTransformStage_[i].scale_ = {4.5f, 1, 1};
		// 変換行列を更新
		worldTransformStage_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformStage_[i].scale_, worldTransformStage_[i].rotation_,
		    worldTransformStage_[i].translation_);
		// 変換行列を定数バッファに転送
		worldTransformStage_[i].TransferMatrix();
	}

	// サウンドデータの読み込み
	soundDataHandleTitleBGM_ = audio_->LoadWave("Audio/Ring05.wav");
	soundDataHandleGamePlayBGM_ = audio_->LoadWave("Audio/Ring08.wav");
	soundDataHandleGameOverBGM_ = audio_->LoadWave("Audio/Ring09.wav");
	soundDataHandleEnemyHitSE_ = audio_->LoadWave("Audio/chord.wav");
	soundDataHandlePlayerHitSE_ = audio_->LoadWave("Audio/tada.wav");

	// タイトルBGMを再生
	voiceHandleBGM_ = audio_->PlayWave(soundDataHandleTitleBGM_, true);
}

// ステージ更新
void GameScene::StageUpdate() {
	// 各ステージでループ
	for (int i = 0; i < 20; i++) {
		// 手前に移動
		worldTransformStage_[i].translation_.z -= 0.1f;
		// 端まで来たら奥へ戻る
		if (worldTransformStage_[i].translation_.z < -5) {
			worldTransformStage_[i].translation_.z += 40;
		}
		// 変換行列を更新
		worldTransformStage_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformStage_[i].scale_, worldTransformStage_[i].rotation_,
		    worldTransformStage_[i].translation_);
		// 変換行列を定数バッファに転送
		worldTransformStage_[i].TransferMatrix();
	}
}

// スコア数値の表示
void GameScene::DrawScore() {
	// 各桁の値をを取り出す
	int eachNumber[5] = {};  // 各桁の値
	int number = gameScore_; // 表示する数字

	int keta = 10000; // 最初の桁
	for (int i = 0; i < 5; i++) {
		eachNumber[i] = number / keta; // 今の桁の値を求める
		number = number % keta;        // 次の桁以下の値を取り出す
		keta = keta / 10;              // 桁を進める
	}

	// 各行の数値を描画
	for (int i = 0; i < 5; i++) {
		spriteNumber_[i]->SetSize({32, 64});
		spriteNumber_[i]->SetTextureRect({32.0f * eachNumber[i], 0}, {32, 64});
		spriteNumber_[i]->Draw();
	}

	spriteScore_->Draw();
}

void GameScene::PlayerUpdate() {
	// 変換行列を更新
	worldTransformPlayer_.matWorld_ = MakeAffineMatrix(
	    worldTransformPlayer_.scale_, worldTransformPlayer_.rotation_,
	    worldTransformPlayer_.translation_);
	// 変換行列を定数バッファに転送
	worldTransformPlayer_.TransferMatrix();

	// 移動

	// 右へ移動
	if (input_->PushKey(DIK_RIGHT)) {
		worldTransformPlayer_.translation_.x += 0.1f;
		if (worldTransformPlayer_.translation_.x > 4) {
			worldTransformPlayer_.translation_.x = 4;
		}
	}

	// 左へ移動
	if (input_->PushKey(DIK_LEFT)) {
		worldTransformPlayer_.translation_.x -= 0.1f;
		if (worldTransformPlayer_.translation_.x < -4) {
			worldTransformPlayer_.translation_.x = -4;
		}
	}

	if (playerTimer_ > 0) {
		playerTimer_ -= 1;
	}
}

void GameScene::BeamUpdate() {
	// 移動
	BeamMove();
	// ビーム発生
	BeamBorn();
	// 変換行列を更新
	for (int i = 0; i < 10; i++) {
		worldTransformBeam_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformBeam_[i].scale_, worldTransformBeam_[i].rotation_,
		    worldTransformBeam_[i].translation_);
		// 変換行列を定数バッファに転送
		worldTransformBeam_[i].TransferMatrix();
	}
}

void GameScene::BeamMove() {
	for (int i = 0; i < 10; i++) {
		if (beamFlag_[i] == 1) {
			worldTransformBeam_[i].translation_.z += 0.3f;
			worldTransformBeam_[i].rotation_.x += 0.1f;
			if (worldTransformBeam_[i].translation_.z > 40) {
				beamFlag_[i] = 0;
			}
		}
	}
}

// ビーム発生（発射）
void GameScene::BeamBorn() {
	// 発射タイマーが0ならば
	if (beamTimer_ == 0) {
		if (input_->PushKey(DIK_SPACE)) {
			for (int i = 0; i < 10; i++) {
				if (beamFlag_[i] == 0) {
					worldTransformBeam_[i].translation_.x = worldTransformPlayer_.translation_.x;
					worldTransformBeam_[i].translation_.y = worldTransformPlayer_.translation_.y;
					worldTransformBeam_[i].translation_.z =
					    worldTransformPlayer_.translation_.z + 0.5f;
					beamFlag_[i] = 1;
					beamTimer_ = 1;
					break;
				}
			}
		}
	} else {
		// 発射タイマーが1以上
		// 10を超えると再び発射が可能
		beamTimer_++;
		if (beamTimer_ > 10) {
			beamTimer_ = 0;
		}
	}
}

void GameScene::EnemyUpdate() {
	// 敵移動
	EnemyMove();
	// 敵発生
	EnemyBorn();
	// 敵ジャンプ
	EnemyJump();
	// 変換行列を更新
	for (int i = 0; i < 10; i++) {
		worldTransformEnemy_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformEnemy_[i].scale_, worldTransformEnemy_[i].rotation_,
		    worldTransformEnemy_[i].translation_);
		// 変換行列を定数バッファに転送
		worldTransformEnemy_[i].TransferMatrix();
	}
}

void GameScene::EnemyMove() {
	for (int i = 0; i < 10; i++) {
		if (enemyFlag_[i] == 1) {
			// タイマーにより速度を設定
			worldTransformEnemy_[i].translation_.z -= 0.1f;
			worldTransformEnemy_[i].translation_.z -= gameTimer_ / 1000.0f;

			worldTransformEnemy_[i].translation_.z -= 0.5f;
			worldTransformEnemy_[i].rotation_.x -= 0.1f;
			worldTransformEnemy_[i].translation_.x += enemySpeed_[i];

			if (worldTransformEnemy_[i].translation_.x > 4) {
				enemySpeed_[i] = -0.1f;
			}
			if (worldTransformEnemy_[i].translation_.x < -4) {
				enemySpeed_[i] = 0.1f;
			}
		}
	}
}

void GameScene::EnemyBorn() {
	// 乱数で発生
	if (rand() % 10 == 0) {
		for (int e = 0; e < 10; e++) {
			if (enemyFlag_[e] == 0) {
				enemyFlag_[e] = 1;
				worldTransformEnemy_[e].translation_.x = 0.0f;
				worldTransformEnemy_[e].translation_.y = 0.0f;
				worldTransformEnemy_[e].translation_.z = 40.0f;
				int x = rand() % 80;          // 80は4の10倍の2倍
				float x2 = (float)x / 10 - 4; // 10で割り、4を引く
				worldTransformEnemy_[e].translation_.x = x2;
				// worldTransformEnemy_[e].translation_.z -= 0.5f;

				if (rand() % 2 == 0) {
					enemySpeed_[e] = 0.1f;
				} else {
					enemySpeed_[e] = -0.1f;
				}

				// ループ終了
				break;
			}

			if (worldTransformEnemy_[e].translation_.z < -10) {
				enemyFlag_[e] = 0;
			}
		}
	}
}

// 敵ジャンプ
void GameScene::EnemyJump() {
	// 敵でループ
	for (int i = 0; i < 10; i++) {
		// 消滅演出ならば
		if (enemyFlag_[i] == 2) {
			// 移動（Y座標に速度を加える）
			worldTransformEnemy_[i].translation_.y += enemyJumpSpeed_[i];
			// 速度を減らす
			enemyJumpSpeed_[i] -= 0.1f;
			// 斜め移動
			worldTransformEnemy_[i].translation_.x += enemySpeed_[i] * 4;
			// 下へ落ちると消滅
			if (worldTransformEnemy_[i].translation_.y < 0) {
				enemyFlag_[i] = 0; // 存在しない
			}
		}
	}
}

void GameScene::ScoreUpUpdate() {
	// 敵移動
	ScoreUpMove();
	// 敵発生
	ScoreUpBorn();
	// 敵ジャンプ
	ScoreUpJump();
	// 変換行列を更新
	for (int i = 0; i < 10; i++) {
		worldTransformScoreUp_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformScoreUp_[i].scale_, worldTransformScoreUp_[i].rotation_,
		    worldTransformScoreUp_[i].translation_);
		// 変換行列を定数バッファに転送
		worldTransformScoreUp_[i].TransferMatrix();
	}
}

void GameScene::ScoreUpMove() {
	for (int i = 0; i < 10; i++) {
		if (enemyFlag_[i] == 1) {
			// タイマーにより速度を設定
			worldTransformScoreUp_[i].translation_.z -= 0.1f;
			worldTransformScoreUp_[i].translation_.z -= gameTimer_ / 1000.0f;

			worldTransformScoreUp_[i].translation_.z -= 0.5f;
			worldTransformScoreUp_[i].rotation_.x -= 0.1f;
			worldTransformScoreUp_[i].translation_.x += scoreUpSpeed_[i];

			if (worldTransformScoreUp_[i].translation_.x > 4) {
				scoreUpSpeed_[i] = -0.1f;
			}
			if (worldTransformScoreUp_[i].translation_.x < -4) {
				scoreUpSpeed_[i] = 0.1f;
			}
		}
	}
}

void GameScene::ScoreUpBorn() {
	// 乱数で発生
	if (rand() % 3 == 0) {
		for (int e = 0; e < 10; e++) {
			if (scoreUpFlag_[e] == 0) {
				scoreUpFlag_[e] = 1;
				worldTransformScoreUp_[e].translation_.x = 0.0f;
				worldTransformScoreUp_[e].translation_.y = 0.0f;
				worldTransformScoreUp_[e].translation_.z = 40.0f;
				int x = rand() % 80;          // 80は4の10倍の2倍
				float x2 = (float)x / 10 - 4; // 10で割り、4を引く
				worldTransformScoreUp_[e].translation_.x = x2;
				// worldTransformEnemy_[e].translation_.z -= 0.5f;

				if (rand() % 2 == 0) {
					scoreUpSpeed_[e] = 0.1f;
				} else {
					scoreUpSpeed_[e] = -0.1f;
				}

				// ループ終了
				break;
			}

			if (worldTransformScoreUp_[e].translation_.z < -10) {
				scoreUpFlag_[e] = 0;
			}
		}
	}
}

// 敵ジャンプ
void GameScene::ScoreUpJump() {
	// 敵でループ
	for (int i = 0; i < 10; i++) {
		// 消滅演出ならば
		if (scoreUpFlag_[i] == 2) {
			// 移動（Y座標に速度を加える）
			worldTransformScoreUp_[i].translation_.y += scoreUpJumpSpeed_[i];
			// 速度を減らす
			scoreUpJumpSpeed_[i] -= 0.1f;
			// 斜め移動
			worldTransformScoreUp_[i].translation_.x += scoreUpSpeed_[i] * 4;
			// 下へ落ちると消滅
			if (worldTransformScoreUp_[i].translation_.y < 0) {
				scoreUpFlag_[i] = 0; // 存在しない
			}
		}
	}
}

void GameScene::Collision() {
	// 衝突判定（プレイヤーと敵）
	CollisionPlayerEnemy();
	CollisionPlayerScoreUp();
	CollisionBeamEnemy();
	CollisionBeamScoreUp();
}

// 衝突判定（プレイヤーと敵）
void GameScene::CollisionPlayerEnemy() {
	// 敵が存在すれば
	for (int i = 0; i < 10; i++) {
		if (enemyFlag_[i] == 1) {
			// 差を求める
			float dx =
			    abs(worldTransformPlayer_.translation_.x - worldTransformEnemy_[i].translation_.x);
			float dz =
			    abs(worldTransformPlayer_.translation_.z - worldTransformEnemy_[i].translation_.z);

			// 衝突したら
			if (dx < 1 && dz < 1) {
				// 存在しない
				enemyFlag_[i] = 0;
				//scoreUpFlag_[i] = 0;
				playerLife_--;
				playerTimer_ = 60;

				// プレイヤーヒットSE
				audio_->PlayWave(soundDataHandlePlayerHitSE_);
			}
		}
	}
}

// 衝突判定（プレイヤーとスコアアップ）
void GameScene::CollisionPlayerScoreUp() {
	// 敵が存在すれば
	for (int i = 0; i < 10; i++) {
		if (scoreUpFlag_[i] == 1) {
			// 差を求める
			float dx =
			    abs(worldTransformPlayer_.translation_.x - worldTransformScoreUp_[i].translation_.x);
			float dz =
			    abs(worldTransformPlayer_.translation_.z - worldTransformScoreUp_[i].translation_.z);

			// 衝突したら
			if (dx < 1 && dz < 1) {
				// 存在しない
				//enemyFlag_[i] = 0;
				scoreUpFlag_[i] = 0;
				playerLife_--;
				playerTimer_ = 60;

				// プレイヤーヒットSE
				audio_->PlayWave(soundDataHandlePlayerHitSE_);
			}
		}
	}
}

// 衝突判定（ビームと敵）
void GameScene::CollisionBeamEnemy() {
	// 敵の数ループする
	for (int e = 0; e < 10; e++) {
		// ビームの数ループする
		for (int b = 0; b < 10; b++) {
			if (beamFlag_[b] == 1 && enemyFlag_[e] == 1) {
				// 敵が存在すれば
				// 差を求める
				float dx = abs(
				    worldTransformBeam_[b].translation_.x - worldTransformEnemy_[e].translation_.x);
				float dz = abs(
				    worldTransformBeam_[b].translation_.z - worldTransformEnemy_[e].translation_.z);

				// 衝突したら
				if (dx < 1 && dz < 1) {
					// 存在しない
					beamFlag_[b] = 0;
					enemyFlag_[e] = 2;
					enemyJumpSpeed_[e] = 1;
					gameScore_++;
					audio_->PlayWave(soundDataHandleEnemyHitSE_);
				}
			}
		}
	}
}

// 衝突判定（ビームとスコアアップ）
void GameScene::CollisionBeamScoreUp() {
	// ループする
	for (int s = 0; s < 10; s++) {
		// ビームの数ループする
		for (int b = 0; b < 10; b++) {
			if (beamFlag_[b] == 1 && enemyFlag_[s] == 1) {
				// 敵が存在すれば
				// 差を求める
				float dx = abs(
				    worldTransformBeam_[b].translation_.x - worldTransformScoreUp_[s].translation_.x);
				float dz = abs(
				    worldTransformBeam_[b].translation_.z - worldTransformScoreUp_[s].translation_.z);

				// 衝突したら
				if (dx < 1 && dz < 1) {
					// 存在しない
					beamFlag_[b] = 0;
					scoreUpFlag_[s] = 2;
					scoreUpJumpSpeed_[s] = 1;
					gameScore_ += 10;
					audio_->PlayWave(soundDataHandleEnemyHitSE_);
				}
			}
		}
	}
}

// ゲームプレイ表示3D
void GameScene::GamePlayDraw3D() {
	for (int i = 0; i < 20; i++) {
		// ステージ
		modelStage_->Draw(worldTransformStage_[i], viewProjection_, textureHandleStage_);
	}
	// プレイヤー
	// modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);
	
	// ビーム
	for (int i = 0; i < 10; i++) {
		if (beamFlag_[i] == 1) {
			modelBeam_->Draw(worldTransformBeam_[i], viewProjection_, textureHandleBeam_);
		}
		// 敵
		if (enemyFlag_[i] == 1) {
			modelEnemy_->Draw(worldTransformEnemy_[i], viewProjection_, textureHandleEnemy_);
		}

		// 敵
		if (enemyFlag_[i] == 2) {
			modelEnemy_->Draw(worldTransformEnemy_[i], viewProjection_, textureHandleEnemy_);
		}
	}

	for (int i = 0; i < 10; i++) {
		if (scoreUpFlag_[i] == 1) {
			modelScoreUp_->Draw(worldTransformScoreUp_[i], viewProjection_, textureHandleScoreUp_);
		}
		// スコアアップ
		if (scoreUpFlag_[i] == 2) {
			modelScoreUp_->Draw(worldTransformScoreUp_[i], viewProjection_, textureHandleScoreUp_);
		}
	}

	if (playerTimer_ % 4 < 2) {
		modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);
	}
}

void GameScene::GamePlayDraw2DBack() {
	// 背景
	spriteBG_->Draw();
}

void GameScene::GamePlayDraw2DNear() {
	// ゲームスコア
	/*char str[100];
	sprintf_s(str, "SCORE %d", gameScore_);
	debugText_->Print(str, 200, 10, 2);*/

	// プレイヤーライフ
	/*sprintf_s(str, "LIFE %d", playerLife_);
	debugText_->Print(str, 900, 10, 2);*/

	for (int i = 0; i < playerLife_; i++) {
		spriteLife_[i]->Draw();
	}

	DrawScore();
}

void GameScene::GamePlayStart() {
	playerLife_ = 3;
	gameScore_ = 0;
	playerTimer_ = 0;
	worldTransformPlayer_.translation_.x = 0.0f;
	for (int i = 0; i < 10; i++) {
		enemyFlag_[i] = 0;
		//enemySpeed_[i] = 0;
		scoreUpFlag_[i] = 0;
		//scoreUpSpeed_[i] = 0;
		beamFlag_[i] = 0;
	}
	GamePlayUpdate();
}

// タイトル更新
void GameScene::TitleUpdate() {
	// エンターキーを押した瞬間
	if (input_->TriggerKey(DIK_RETURN)) {
		// モードをゲームプレイへ変更
		sceneMode_ = 0;
		GamePlayStart();

		// BGM切り替え
		audio_->StopWave(voiceHandleBGM_); // 現在のBGMを停止
		voiceHandleBGM_ = audio_->PlayWave(soundDataHandleGamePlayBGM_, true);
	}
}

// タイトル表示
void GameScene::TitleDraw2DNear() {
	// タイトル表示
	spriteTitle_->Draw();

	// エンター表示
	if (gameTimer_ % 40 >= 20) {
		spriteEnter_->Draw();
	}
}

// ゲームオーバー更新
void GameScene::GameOverUpdate() {
	if (playerLife_ == 0) {
		sceneMode_ = 2;
	}
}

// ゲームオーバー表示
void GameScene::GameOverDraw2DNear() {
	spriteGameOver_->Draw();

	if (sceneMode_ == 2) {
		if (input_->TriggerKey(DIK_RETURN)) {
			sceneMode_ = 1;
		}
	}

	// エンター表示
	if (gameTimer_ % 40 >= 20) {
		spriteEnter_->Draw();
	}
}

void GameScene::Update()
{
	// 各シーンの更新処理を呼び出す
	switch (sceneMode_) {
	case 0:
		GamePlayUpdate(); // ゲームプレイ更新
		GameOverUpdate(); // ゲームオーバー更新
		if (playerLife_ == 0) {
			audio_->StopWave(voiceHandleBGM_);
			voiceHandleBGM_ = audio_->PlayWave(soundDataHandleGameOverBGM_, true);
		}
		break;
	case 1:
		TitleUpdate(); // タイトル更新
		gameTimer_++;

		break;
	case 2:
		gameTimer_ += 1;
		if (input_->PushKey(DIK_RETURN)) {
			audio_->StopWave(voiceHandleBGM_);
			voiceHandleBGM_ = audio_->PlayWave(soundDataHandleTitleBGM_, true);
		}

		break;
	}
}

void GameScene::GamePlayUpdate() {
	PlayerUpdate(); // プレイヤー更新
	EnemyUpdate();  // 敵更新
	ScoreUpUpdate();  // スコアアップ更新
	BeamUpdate();   // ビーム更新
	StageUpdate();  // ステージ更新
	Collision();    // 衝突判定
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>
	
	switch (sceneMode_) {
	case 0:
		GamePlayDraw2DBack();
		break;
	case 2:
		GamePlayDraw2DBack();
	}

	spriteBG_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	
	switch (sceneMode_) {
	case 0:
		GamePlayDraw3D(); // ゲームプレイ3D表示
		break;
	case 2:
		GamePlayDraw3D();
	}

	for (int i = 0; i < 20; i++) {
		// ステージ
		modelStage_->Draw(worldTransformStage_[i], viewProjection_, textureHandleStage_);
	}
	// プレイヤー
	// modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);

	for (int i = 0; i < 10; i++) {
		// ビーム
		if (beamFlag_[i] == 1) {
			modelBeam_->Draw(worldTransformBeam_[i], viewProjection_, textureHandleBeam_);
		}

		// 敵
		if (enemyFlag_[i] == 1) {
			modelEnemy_->Draw(worldTransformEnemy_[i], viewProjection_, textureHandleEnemy_);
		}
	}

	for (int i = 0; i < 10; i++) {
		// スコアアップ
		if (scoreUpFlag_[i] == 1) {
			modelScoreUp_->Draw(worldTransformScoreUp_[i], viewProjection_, textureHandleScoreUp_);
		}
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	
	debugText_->DrawAll();

	switch (sceneMode_) {
	case 0:
		GamePlayDraw2DNear(); // ゲームプレイ2D近景表示
		break;
	case 1:
		TitleDraw2DNear();
		break;
	case 2:
		GameOverDraw2DNear();
		gameTimer_++;
		GamePlayDraw2DNear();
		break;
	}

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
