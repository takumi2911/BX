"""
scripts/ue5_python/create_blueprints_batch.py

UE5 エディタの Python Console または Editor Utility から実行する想定。
C++ 基底クラスを親にした Blueprint を一括作成する。

使い方:
1. UE5 エディタ起動
2. Window → Developer Tools → Output Log
3. Output Log 下部の Cmd ドロップダウンを "Python" に
4. このスクリプトを貼り付けて実行

または:
- Edit → Project Settings → Plugins → Python Editor Script Plugin を有効化
- Tools → Execute Python Script でこのファイルを指定
"""
import unreal


# ------------------------------------------------------------
# 作成したい BP のリスト: (BP名, 親クラスパス, 保存先パス)
# ------------------------------------------------------------
BLUEPRINTS_TO_CREATE = [
    # Player & Characters
    ("BP_BX_Player",    "/Script/BX.PlayerCharacterBase",    "/Game/BX/Characters"),
    ("BP_BX_AI_Looter", "/Script/BX.EnemyCharacterBase",     "/Game/BX/Characters"),

    # Core
    ("BP_BX_GameMode_Raid",        "/Script/BX.GameModeRaid_BX",    "/Game/BX/Core"),
    ("BP_BX_GameMode_MerchantHub", "/Script/BX.GameModeHub_BX",     "/Game/BX/Core"),

    # Weapons (基底 AWeaponBase から派生)
    ("BP_BX_Weapon_Pistol9mm",  "/Script/BX.WeaponBase", "/Game/BX/Weapons"),
    ("BP_BX_Weapon_SMG9mm",     "/Script/BX.WeaponBase", "/Game/BX/Weapons"),
    ("BP_BX_Weapon_AR556",      "/Script/BX.WeaponBase", "/Game/BX/Weapons"),

    # Interactables
    ("BP_BX_LootContainer_Warehouse01", "/Script/BX.InteractableBase", "/Game/BX/Blueprints"),
    ("BP_BX_Door_Standard",             "/Script/BX.InteractableBase", "/Game/BX/Blueprints"),
]


def create_blueprint(bp_name: str, parent_class_path: str, target_folder: str):
    """指定の親クラスから Blueprint を作成して保存する"""

    # 重複チェック
    full_asset_path = f"{target_folder}/{bp_name}"
    if unreal.EditorAssetLibrary.does_asset_exist(full_asset_path):
        unreal.log_warning(f"Skipped (already exists): {full_asset_path}")
        return None

    # 親クラスをロード
    parent_class = unreal.load_class(None, parent_class_path)
    if parent_class is None:
        unreal.log_error(f"Parent class not found: {parent_class_path}")
        return None

    # Blueprint Factory の設定
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", parent_class)

    # AssetTools で作成
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    new_bp = asset_tools.create_asset(
        asset_name=bp_name,
        package_path=target_folder,
        asset_class=unreal.Blueprint,
        factory=factory,
    )

    if new_bp is None:
        unreal.log_error(f"Failed to create: {full_asset_path}")
        return None

    # 保存
    unreal.EditorAssetLibrary.save_loaded_asset(new_bp)
    unreal.log(f"Created: {full_asset_path}")
    return new_bp


def main():
    created = 0
    skipped = 0
    failed = 0

    for bp_name, parent_path, target_folder in BLUEPRINTS_TO_CREATE:
        result = create_blueprint(bp_name, parent_path, target_folder)
        if result is None:
            if unreal.EditorAssetLibrary.does_asset_exist(f"{target_folder}/{bp_name}"):
                skipped += 1
            else:
                failed += 1
        else:
            created += 1

    unreal.log("=" * 50)
    unreal.log(f"Created: {created}, Skipped: {skipped}, Failed: {failed}")
    unreal.log("=" * 50)


if __name__ == "__main__":
    main()
