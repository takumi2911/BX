Sprint 12 の C++ 部分を開始してください。



\# Sprint 12 目的(C++ 部分)



Sprint 11 で作成した APlayerCharacterBase の Input\_\* スタブ関数に、Enhanced Input 経由で実際にプレイヤーが動くように実装を追加します。



UE5 エディタ側の作業(IA\_BX\_\* アセット作成、IMC 作成、BP\_BX\_Player 派生作成、レベル作成)は、Sprint 12 の後半でユーザーが手作業でやります。今回は C++ のみに集中してください。



\# 参照ドキュメント



1\. docs/SPEC\_PartB.md の §23-4(プレイヤー基底クラス)の入力アクション定義

2\. Source/BX/Public/Characters/APlayerCharacterBase.h(Sprint 11 の実装)

3\. Source/BX/Private/Characters/APlayerCharacterBase.cpp(Sprint 11 のスタブ実装)



\# Sprint 12 (C++) スコープ



\## 1. 必要な UE5 module の確認



APlayerCharacterBase.cpp で Enhanced Input を使うので、以下が必要:

\- BX.Build.cs の PublicDependencyModuleNames に "EnhancedInput" を追加(なければ)

\- APlayerCharacterBase.cpp で #include "EnhancedInputComponent.h" と #include "EnhancedInputSubsystems.h"



\## 2. Input\_\* 関数のスタブを実装



以下の関数を、空実装 + UE\_LOG から、実際に動く実装に変更:



\### Input\_MoveTriggered(const FInputActionValue\& Value)

\- Value.Get<FVector2D>() で 2D 入力取得

\- GetActorForwardVector() / GetActorRightVector() を取得

\- AddMovementInput(ForwardVector, MoveValue.Y) / AddMovementInput(RightVector, MoveValue.X)



\### Input\_LookTriggered(const FInputActionValue\& Value)

\- Value.Get<FVector2D>() で 2D 入力取得

\- AddControllerYawInput(LookValue.X)

\- AddControllerPitchInput(LookValue.Y)



\### Input\_JumpTriggered()

\- ACharacter::Jump() を呼ぶ



\### Input\_SprintStarted() / Input\_SprintCompleted()

\- CurrentLocomotionState を Sprint / Walk(または Idle)に切り替え

\- GetCharacterMovement()->MaxWalkSpeed を変更(Sprint 時は 600, 通常は 300 など)



\### Input\_CrouchToggled()

\- ACharacter::Crouch() / UnCrouch() をトグル



その他の Input\_\* 関数(LeanLeft, Reload, Fire 等)は、まだ対応するシステムが未実装なので、UE\_LOG で「未実装」と出すだけで OK。



\## 3. BindAllInputActions(UEnhancedInputComponent\*) の実装



引数で渡された InputComponent に対して、各 IA\_BX\_\* と Input\_\*関数をバインド:



```cpp

if (InputComponent \&\& IA\_BX\_Move)

{

&#x20;   InputComponent->BindAction(IA\_BX\_Move, ETriggerEvent::Triggered, this, \&APlayerCharacterBase::Input\_MoveTriggered);

}

if (InputComponent \&\& IA\_BX\_Look)

{

&#x20;   InputComponent->BindAction(IA\_BX\_Look, ETriggerEvent::Triggered, this, \&APlayerCharacterBase::Input\_LookTriggered);

}

if (InputComponent \&\& IA\_BX\_Jump)

{

&#x20;   InputComponent->BindAction(IA\_BX\_Jump, ETriggerEvent::Started, this, \&APlayerCharacterBase::Input\_JumpTriggered);

}

if (InputComponent \&\& IA\_BX\_Sprint)

{

&#x20;   InputComponent->BindAction(IA\_BX\_Sprint, ETriggerEvent::Started, this, \&APlayerCharacterBase::Input\_SprintStarted);

&#x20;   InputComponent->BindAction(IA\_BX\_Sprint, ETriggerEvent::Completed, this, \&APlayerCharacterBase::Input\_SprintCompleted);

}

if (InputComponent \&\& IA\_BX\_Crouch)

{

&#x20;   InputComponent->BindAction(IA\_BX\_Crouch, ETriggerEvent::Started, this, \&APlayerCharacterBase::Input\_CrouchToggled);

}

```



その他の IA\_\* も同様にバインドしてください(対応するスタブ関数があるもの全部)。



\## 4. SetupPlayerInputComponent(UInputComponent\* PlayerInputComponent) のオーバーライド



ACharacter::SetupPlayerInputComponent をオーバーライドして、UEnhancedInputComponent にキャストし、BindAllInputActions を呼ぶ:



```cpp

void APlayerCharacterBase::SetupPlayerInputComponent(UInputComponent\* PlayerInputComponent)

{

&#x20;   Super::SetupPlayerInputComponent(PlayerInputComponent);



&#x20;   if (UEnhancedInputComponent\* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))

&#x20;   {

&#x20;       BindAllInputActions(EnhancedInput);

&#x20;   }

}

```



\## 5. ビルド検証(必須)



ビルドが通ることを確認してください。



\# Sprint 12 (C++) 完了基準



\- BX.Build.cs に EnhancedInput が含まれている

\- 主要 Input\_\* 関数(Move/Look/Jump/Sprint/Crouch)が実装されている

\- BindAllInputActions が実装されている

\- SetupPlayerInputComponent がオーバーライドされている

\- ビルドが通る



\# Sprint 12 (C++) でやらないこと



\- IA\_BX\_\* アセットの作成(エディタ作業)

\- IMC\_BX\_Default の作成(エディタ作業)

\- BP\_BX\_Player の作成(エディタ作業)

\- レベルの作成(エディタ作業)



\# 報告



完了時に以下を報告:

1\. 変更したファイルパスのリスト

2\. ビルド成功 / 失敗

3\. 実装した Input\_\* 関数のリスト

4\. 未実装で UE\_LOG だけにした Input\_\* 関数のリスト

5\. 次にユーザーがやるべき UE5 エディタ作業(箇条書き)



最後に commit + push をしてください。

コミットメッセージ: feat: Sprint 12 (C++) - APlayerCharacterBase の Input\_\* 実装と BindAllInputActions

