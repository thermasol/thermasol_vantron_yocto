# vt-sbc-am62l-yocto

Yocto/OpenEmbedded BSP for Vantron's **VT-SBC-AM62L** (TI AM62L SoC), Yocto "scarthgap" release.

Flattened snapshot of Vantron's `repo`-managed workspace (manifest `scarthgap-VT-SBC-AM62L-v1.0.7.xml`), pulled from
`git.vantrontech.us` via:

```bash
repo init -u ssh://git@git.vantrontech.us:8222/linux/ti/6.12/standard/VT-SBC-AM62L/manifests-yocto.git \
  -b scarthgap -m scarthgap-VT-SBC-AM62L-v1.0.7.xml \
  --repo-url ssh://git@git.vantrontech.us:8222/linux/git-repo.git
repo sync
```

Each layer's nested `.git` history has been stripped and everything committed here as one flat tree, since
`git.vantrontech.us` access isn't guaranteed to persist.

## Layers (manifest v1.0.7 pinned revisions)

| Layer | Revision |
|---|---|
| poky | `200d12b6a58ad961d60a7774ca0f7a9d29498724` |
| meta-arm | `950a4afce46a359def2958bd9ae33fc08ff9bb0d` |
| meta-clang | `eea5ec23155cfd47781599fdc9a92c21e4caffc8` |
| meta-lts-mixins | `a8046d5ec53b1856169ac795aa87cb0d5db84c04` |
| meta-openembedded | `72018ca1b1a471226917e8246e8bbf9a374ccf97` |
| meta-qt6 | `586a6cb5aec755803a3be3cec359baafe89d6432` |
| meta-ti | `59354be6c2362b9f7d0a3cd2c017f2f3c6ab7f2e` |
| meta-vantron/meta-custom | `ad854292641b5f4804ae74408e2e8dbc0c4e5177` |
| meta-vantron/meta-vtlinux-base | `1dde6939e4971428f41ca07ab8ed3b5f3cae4ea6` |

## Known deviation from Vantron upstream

`layers/meta-vantron/meta-vtlinux-base/recipes-samples/eeprom-util/eeprom-util_3.2.0.bb` originally fetched from
`git.vantrontech.us:8222/linux/vantron/applications/eeprom-utils.git`, which this GitHub account has no read
access to. The recipe's own `HOMEPAGE` and the already-vendored `Allow-cross-compilation.patch` (a 3-line Makefile
tweak) show Vantron's fork is just upstream CompuLab's `eeprom-util` v3.2.0 — confirmed by an exact `LICENSE`
checksum match. `SRC_URI`/`SRCREV` were repointed at the public
[`compulab/eeprom-util`](https://github.com/compulab/eeprom-util) repo, tag `v3.2.0`
(`257db099e8339404510eded3521da5370b321041`), instead.

## Build

```bash
source layers/poky/oe-init-build-env build
bitbake <image-name>
```
