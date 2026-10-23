<div align="center">

# BliKVM

### Your monitor, keyboard, and mouse. In your browser.

**See the screen, control the computer, install an OS, and manage power remotely—with no agent software on the target computer.**

**English · [简体中文](README_CN.md)**

[Online Demo](https://blikvm.com/demo/) · [Downloads & Updates](https://github.com/blikvm/blikvm/releases) · [Documentation](https://blikvm.com/docs/) · [Hardware & Accessories](https://blicube.aliexpress.com/) · [Report an Issue](https://github.com/blikvm/blikvm/issues) · [Discord](https://discord.gg/9Y374gUF6C)

</div>

BliKVM is a hardware and software KVM over IP solution. Connect it to a computer's HDMI and USB ports, add a network connection, and use your browser to see the actual display and operate its keyboard and mouse.

From BIOS/UEFI and boot menus to the operating system desktop, control does not depend on remote desktop software running on the target. When its OS will not boot, its network configuration fails, or it needs a fresh installation, an independently powered and connected BliKVM can still help you recover it.

**Try it first:** [Open the online demo](https://blikvm.com/demo/) to explore the BliKVM interface.

![BliKVM demo interface showcase with virtual keyboard, settings, and Virtual Media](images/blikvm-readme-hero.png)

> This page describes the new BliKVM software. Available features, firmware availability, and upgrade compatibility vary by model. Refer to the relevant release notes.

## Why BliKVM?

| What you need | How BliKVM helps |
| --- | --- |
| **Access when the OS cannot help** | Reach the HDMI display and USB keyboard and mouse for BIOS setup, boot troubleshooting, and OS installation. |
| **Hardware-encoded live video** | Operate through H.264 and WebRTC, with image quality, bitrate, and frame rate settings. MJPEG is also available. |
| **Rich OpenAPI support for AI and automation** | Connect screen capture, keyboard and mouse input, power control, and other APIs to your own programs so AI agents and automation tools can operate physical computers. |
| **USB workflows beyond keyboard and mouse** | Configure virtual media, USB networking, and camera and microphone forwarding as needed. |
| **Your choice of network access** | Use BliKVM independently on your LAN, or configure WireGuard, Tailscale, or ZeroTier for your remote network. |
| **Updates with a recovery path** | The new system uses A/B updates with signature verification, boot health confirmation, and fallback. The App supports independent updates and rollback. |
| **Multiple platforms, a familiar interface** | CM4, Pi 4B, H616, and RV1126B share a consistent Web interface, with settings shown according to device capabilities. |

## From everyday control to remote recovery

### Video, audio, and control in one browser

- View the target computer's HDMI output and use keyboard, mouse, and common shortcuts.
- Stream hardware-encoded H.264 video, with H.265 on supported models.
- Adjust video settings to suit image quality, bandwidth, and responsiveness.
- Listen to the target computer through supported audio capture paths.
- Use fullscreen, screenshots, browser recording, text input, and keyboard macros.
- Customize your workspace with languages, themes, and toolbar settings.

### More than a remote keyboard and mouse

Choose USB functions for the task at hand:

| Function | What it enables |
| --- | --- |
| **Keyboard and mouse** | Absolute or relative mouse modes for different operating systems and workflows. |
| **Virtual media** | Present a virtual optical drive or storage device for OS installation, recovery, and maintenance. |
| **USB virtual network** | Provide compatible target systems with a USB network connection and optionally share the KVM's network. |
| **Browser camera and microphone forwarding** | Make your remote browser's camera or microphone available to the target computer. |
| **External USB camera** | Add a view of the site, instruments, or rack environment on supported devices. |
| **Local USB keyboard and mouse forwarding** | Forward physical input devices plugged into the KVM on supported hardware. |

Simultaneous combinations depend on USB controller resources and firmware support. The interface checks relevant capabilities and resource limits. See the documentation for your model.

### Maintenance tools within reach

- **Power management:** Use an ATX connection to control power and restart, or Wake-on-LAN to wake compatible systems.
- **Terminal and serial console:** Manage the KVM itself in your browser, or troubleshoot equipment through a configured serial connection.
- **Automation:** Combine keyboard macros, custom scripts, and APIs to integrate repetitive tasks into your workflow.
- **Monitoring and diagnostics:** Inspect status, network addresses, logs, and diagnostics, with Prometheus integration for monitoring.
- **Network management:** Configure Ethernet, supported Wi-Fi, VPNs, and DDNS for your deployment.

### Rich OpenAPI support for AI-driven computer automation

BliKVM provides both a browser interface and a broad set of APIs described with **OpenAPI 3**. They cover screenshots, keyboard and mouse input, text and shortcuts, power control, virtual media, and device management. Use your preferred language and tools to connect these capabilities to an operations platform, automation script, or AI agent.

Combined with an AI model that can interpret images, the APIs enable a feedback loop:

1. **Observe:** Capture the computer's current screen.
2. **Understand and plan:** Let your AI analyze the interface and choose the next action.
3. **Act:** Send keystrokes, pointer movement, clicks, or text through the APIs.
4. **Verify:** Capture the screen again to check the result and decide what to do next.

Automation can extend beyond the target computer's existing remote desktop services or software APIs. Build workflows for graphical interfaces, installation checks, repetitive maintenance, or assisted BIOS/UEFI operation. BliKVM provides hardware access and control; you choose and integrate the AI model and task orchestration.

**Start with your device's API documentation:** Visit `https://<device-address>/docs` for interactive documentation, or retrieve `https://<device-address>/openapi.yaml` to generate clients and explore request parameters. API calls use an authorized device API token and are subject to account permissions and hardware capabilities.

### Access and permissions under your control

BliKVM supports HTTPS, account permissions, two-factor authentication, and revocable API tokens. Assign administrator, operator, or viewer access and enable the remote access methods you need.

LAN use does not require a BliKVM cloud account. VPNs, online services, and optional cloud features depend on the model and firmware and may require network configuration or service accounts.

## Updates for long-term use

The new BliKVM software maintains the system and App separately, so routine application updates do not require reflashing the storage device.

- **A/B system updates:** Write the new system to the inactive slot and confirm its health after reboot. Failed trial boots can fall back to the previous slot through the update mechanism.
- **System and bundled App switching:** System updates also handle the included App, helping prevent a new system from continuing to run an incompatible old application.
- **Independent App updates and rollback:** Install an App update package when only the interface or application needs updating.
- **Pre-update checks:** Verify package signatures, hardware compatibility, versions, and compatibility requirements. Use release checksums to verify downloads.
- **Persistent configuration:** User configuration and working data reside on persistent storage during routine updates. Back them up separately before factory resets or reflashing.

System and App version numbers may differ. Check the versions on your About page and the release compatibility notes before choosing an update package.

## Hardware and purchasing

BliKVM offers hardware based on Raspberry Pi CM4, Pi 4B, Allwinner H616, and Rockchip RV1126B, plus switches, ATX accessories, and capture boards.

**[Visit the official store for all products and accessories](https://blicube.aliexpress.com/)**

See the [official website](https://blikvm.com/) for model details and specifications. Video, USB resources, and expansion capabilities vary. Match the image to your hardware model and capture chip.

## Download and get started

**[Download from GitHub Releases](https://github.com/blikvm/blikvm/releases)**

Identify your hardware first, then choose a file for your task. Available files depend on the selected release:

| Your task | File type | How to use it |
| --- | --- | --- |
| **First installation, reinstallation, or migration from an older system** | Full image, such as `.img.xz` / `.img.gz` | Decompress and flash to the appropriate storage medium as instructed. The image includes the App. |
| **Update a compatible A/B system** | System update package, `.raucb` | Upload it in the system update interface and follow the instructions. |
| **Update only the App** | App update package, `.tar.gz` | Install it in the application update interface after checking minimum system requirements. |

Older releases may use different partition layouts, package formats, and update methods. Follow the target release's migration instructions rather than inferring compatibility from filenames or version numbers.

### Four steps to remote control

1. **Connect:** Connect the target computer's HDMI output and USB port to BliKVM. Add ATX wiring if you need power control.
2. **Add network and power:** Connect the KVM to your network and power it according to its model's instructions.
3. **Open your browser:** Find its address on the device display or your router's DHCP list. Open the HTTPS interface and complete first-time login and password setup.
4. **Take control:** Open the video stream and test keyboard and mouse input. Enable audio, virtual media, or other USB functions as needed.

See the [documentation](https://blikvm.com/docs/) for wiring, flashing, and model differences. Where new firmware differs from older documentation, follow the relevant release notes.

## About this repository

`blikvm/blikvm` is BliKVM's public product and release hub. This branch contains the English and Chinese READMEs and their supporting images, with links to downloads, release notes, documentation, hardware, and community support.

Historical source code, build scripts, images, and development notes remain available in [Git history](https://github.com/blikvm/blikvm/commits/master/). They are not the build entry point for current firmware. Removing them from the current tree does not change their existing licenses.

## Documentation, support, and feedback

- **Online experience:** [BliKVM demo](https://blikvm.com/demo/)
- **OpenAPI documentation:** `/docs` on your device; interface definition at `/openapi.yaml`.
- **Official website:** [blikvm.com](https://blikvm.com/)
- **User documentation:** [BliKVM docs](https://blikvm.com/docs/)
- **Downloads and release notes:** [GitHub Releases](https://github.com/blikvm/blikvm/releases)
- **Issues and feature requests:** [GitHub Issues](https://github.com/blikvm/blikvm/issues)
- **Community:** [Discord](https://discord.gg/9Y374gUF6C)
- **Product inquiries and customization:** [info@blicube.com](mailto:info@blicube.com)

When reporting a problem, include your hardware model, system version, App version, reproduction steps, and relevant logs.

## Acknowledgments and licenses

Thank you to [PiKVM](https://github.com/pikvm/pikvm), [TinyPilot](https://github.com/tiny-pilot/tinypilot), and the open-source projects supporting the KVM ecosystem.

The license for historical code is preserved in the [historical LICENSE file](https://github.com/blikvm/blikvm/blob/3e34c04a1dfa84636050585d12d5e585098258ea/LICENSE). Components and third-party dependencies in release packages retain their respective licenses and terms.
