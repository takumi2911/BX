"""
scripts/ue5_python/reimport_datatables.py

UE5 エディタで DataSource/*.csv と対応する DT_BX_* の Reimport を一括実行。

前提:
- 対応 DT が既にエディタで作成済み(初回は手動で作る必要あり)
- DT の Source File が CSV を指している

使い方: UE5 エディタ Output Log の Python Cmd で実行。
"""
import unreal

# DT とソース CSV の対応表
DT_CSV_MAPPING = [
    ("/Game/BX/Data/Common/DT_BX_Weapons",  "DataSource/weapons.csv"),
    ("/Game/BX/Data/Common/DT_BX_Ammo",     "DataSource/ammo.csv"),
    ("/Game/BX/Data/Common/DT_BX_Armor",    "DataSource/armor.csv"),
    ("/Game/BX/Data/Common/DT_BX_Medical",  "DataSource/medical.csv"),
    ("/Game/BX/Data/Common/DT_BX_Items",    "DataSource/items.csv"),
    # 地域別
    ("/Game/BX/Data/Regions/KansaiIndustrial01/DT_BX_Loot_KansaiIndustrial01",
     "DataSource/regions/kansai_industrial_01/loot.csv"),
]


def reimport_datatable(dt_path: str):
    """指定 DT を Reimport"""
    dt = unreal.load_asset(dt_path)
    if dt is None:
        unreal.log_warning(f"DT not found: {dt_path}")
        return False

    # AssetTools で Reimport
    unreal.AssetToolsHelpers.get_asset_tools().reimport_assets([dt])
    unreal.EditorAssetLibrary.save_loaded_asset(dt)
    unreal.log(f"Reimported: {dt_path}")
    return True


def main():
    success = 0
    failed = 0
    for dt_path, csv_hint in DT_CSV_MAPPING:
        if reimport_datatable(dt_path):
            success += 1
        else:
            failed += 1

    unreal.log("=" * 50)
    unreal.log(f"Reimport: Success={success}, Failed={failed}")
    unreal.log("=" * 50)


if __name__ == "__main__":
    main()
