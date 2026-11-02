# Phase 13 local Linux environment

Status: recommendation exercised successfully for Phase 13. The existing Docker Desktop Linux VM hosted the local Ubuntu container; no separate VM application or host package installation was needed.

## Recommendation

Use the existing local Docker Desktop Linux ARM64 engine for a disposable Ubuntu ARM64 container. Docker Desktop already provides the Linux VM; this avoids adding a VM manager to the Mac while giving the project the Linux user space it needs. The Mac and engine are ARM64, so Ubuntu ARM64 runs natively. In that guest, the GNU PowerPC cross compiler builds the project’s 32-bit big-endian binaries and `qemu-ppc` user emulation runs them. The project already documents the `ubuntu:26.04` container recipe on Mac and uses this toolchain and execution model in CI. This is not physical PowerPC hardware evidence.

Do not create an x86 VM or a full PowerPC system VM. They do not match the checked-in test path. UTM is the best fallback if a persistent, visible VM is preferred; Lima is a good CLI-first VM but adds a host dependency. VirtualBox is unnecessary here. Apple Silicon VM guests should be ARM64, and UTM confirms matching-architecture guests can use hardware acceleration. QEMU user mode is specifically designed to execute a different-architecture program on the same OS and accepts a library root via `-L`.

| Route | Fit for this work | Recommendation |
|---|---|---|
| Existing Docker Desktop ARM64 engine + Ubuntu ARM64 container | Already present; scriptable; disposable; provides Linux packages and `qemu-ppc` without a new Mac app | Preferred after disk preflight |
| UTM + Ubuntu ARM64 VM | Persistent GUI VM, with `utmctl` lifecycle scripting; initial guest setup remains more involved | Use if a full VM is specifically preferred |
| Lima + Ubuntu ARM64 VM | Declarative, CLI-first VM setup using Apple Virtualization | Use if VM lifecycle automation is more important than avoiding another host tool |
| VirtualBox, x86 Linux guest, or full PowerPC system emulation | Extra setup or architecture emulation not required by the repository | Do not use for this acceptance route |

Docker containers share the Linux VM kernel, unlike a separately managed UTM/Lima guest. They still provide the Linux user space required by the existing cross-build/QEMU test lane. Docker Desktop’s local Linux engine reported `linux/aarch64`; no remote context or container was started during research.

## Why the ARM64 Linux route matches the project

The repository toolchain targets Linux `powerpc`, uses `powerpc-linux-gnu-gcc/g++`, expects `/usr/powerpc-linux-gnu`, and runs through `qemu-ppc -L /usr/powerpc-linux-gnu`. The project workflow installs `gcc-powerpc-linux-gnu`, `g++-powerpc-linux-gnu`, and `qemu-user`. Ubuntu publishes those cross compiler and QEMU packages for ARM64. QEMU user mode executes the produced PowerPC programs under the Linux guest kernel.

The project’s CMake minimum is 4.3.0. Ubuntu 26.04’s package page lists CMake 4.2.3, so the setup must use the repository’s pinned `cmake-linux-aarch64` (currently 4.4.3), not blindly install Ubuntu’s CMake package. Ubuntu ARM64 also publishes clang 21 and its sanitizer runtime packages, which match the project’s ASan/UBSan lane; package install and runtime still need to be confirmed in the actual setup.

## Bounded setup and evidence plan

1. Recheck the active Docker context is the local `desktop-linux` ARM64 engine. Check Docker’s allocated storage and the host volume before pulling anything. The host had about 9.9 GiB free at research time. No cleanup or pruning is authorized by this plan. If the available store is too small for the base image, toolchain, pins, and separate build trees, stop and use an owner-selected external volume or wait until space is available.
2. Use the repository-documented `ubuntu:26.04` image with `--platform linux/arm64`, and record the resolved image ID/digest. Create a named container with no privileged mode, Docker socket, SSH agent, or live checkout mount. Keep network available for provisioning; run tests without network if the workflows permit. Set resource limits only after checking the host’s actual capacity and the project’s existing timeout behavior.
3. Create a Git bundle from the clean exact source commit `d37f4ccfaa33d4cf2966a72a78bad32a46b9bf7c`, transfer it into the container, clone it there, and verify `git rev-parse HEAD`. This excludes the later planning-only commits and avoids sharing the mutable host worktree. Do not use `git archive`, because the checks inspect Git commit/tree metadata.
4. Follow `docs/testing.md` for the base packages (`ca-certificates curl xz-utils git ninja-build gcc g++ binutils`), plus `gcc-powerpc-linux-gnu`, `g++-powerpc-linux-gnu`, `qemu-user`, and for ASan `clang-21 lld-21 libclang-rt-21-dev`. Install the repository-pinned ARM64 CMake and fetch only the fixture pins required by the native, BE, and sanitizer workflows using `scripts/fetch-pin.sh`; verify each against `pins.txt`. Confirm the compiler sysroot and `qemu-ppc` before the full inventories. Ubuntu’s apt CMake package is below the project minimum.
5. Run the existing native `ci` and `be32-full` workflows against the same verified commit and use the repository’s inventory/equality scripts. Then run the complete `asan` workflow using clang 21 and its runtime. Keep state bytes and build outputs within the container/worktree; export only command, exit status, commit identity, inventory/equality receipts, and sanitizer outcome. The exact-source local run can establish QEMU-executed BE behavior and native/BE equality, but it cannot create a GitHub Actions run URL or establish physical PPC hardware behavior.
6. Preserve the container and receipts until the acceptance gap is reviewed. Remove only the named container and its named image/cache if the owner later requests cleanup; do not run global prune commands.

The fixture pins are a separate download boundary from installing the Linux base image and OS packages. Full equality/sanitizer evidence needs those verified pins. The recommendation itself does not grant permission to pull the image, install packages, fetch pins, or publish a workflow; the Phase 13 setup below proceeded after the owner's separate authorization.

## Phase 13 execution outcome

The owner authorized the bounded local setup. The `ubuntu:26.04` ARM64 image was already cached, so no image pull was needed. A non-privileged container with no host checkout mount was provisioned on the local `desktop-linux` engine. The exact source commit `13bb1af4e7b63fb727854d42ad4431091f2083cc` was tested inside it using the pinned ARM64 CMake, the PowerPC cross compiler and `qemu-ppc`, and clang 21 with ASan/UBSan. The complete native and PowerPC/QEMU normal and release inventories, the sanitizer inventory, and exact-source equality checks passed. This establishes the repository's Linux/QEMU acceptance path, not physical PowerPC hardware behavior. Command outcomes and image identity are recorded in the phase's [resource evidence](../phases/13-portable-save-load-across-three-interfaces/13-RESOURCE-EVIDENCE.md).

## Sources

- [Apple: creating and running a Linux VM](https://developer.apple.com/documentation/virtualization/creating-and-running-a-linux-virtual-machine)
- [UTM: QEMU system and acceleration settings](https://docs.getutm.app/settings-qemu/qemu/)
- [UTM: scripting and `utmctl`](https://docs.getutm.app/scripting/scripting/)
- [QEMU: user mode emulation](https://www.qemu.org/docs/master/user/index.html) and [the `-L` library-root option](https://www.qemu.org/docs/master/user/main.html?from=20423&from_column=20423)
- [Ubuntu ARM64 PowerPC cross compiler packages](https://packages.ubuntu.com/en/resolute/devel/gcc-powerpc-linux-gnu), [C++ cross compiler](https://packages.ubuntu.com/resolute/g%2B%2B-powerpc-linux-gnu), and [ARM64 `qemu-user` files](https://packages.ubuntu.com/resolute/arm64/qemu-user/filelist)
- [Ubuntu 26.04 ARM64 CMake package](https://packages.ubuntu.com/resolute/arm64/cmake)
- [Ubuntu ARM64 clang package search](https://packages.ubuntu.com/search?arch=arm64&keywords=clang&searchon=all&suite=resolute) and [clang 21 sanitizer runtime](https://packages.ubuntu.com/resolute/libclang-rt-21-dev)
- [Docker Desktop on Mac FAQ](https://docs.docker.com/desktop/troubleshoot-and-support/faqs/macfaqs/) and [Docker containers versus VMs](https://docs.docker.com/learn/content/what-is-a-container/)
- Repository recipe and contract: [testing guide](../../docs/testing.md), [PowerPC toolchain](../../cmake/toolchains/ppc-linux-gnu.cmake), [equality workflow](../../.github/workflows/be-equality.yml), and [pins](../../pins.txt)
