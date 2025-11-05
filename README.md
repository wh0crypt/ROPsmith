# ROPsmith

<div align="center">
<pre>
:::::::..       ...   ::::::::::.                         `::   ::        
;;;;``;;;;   .;;;;;;;. `;;;```.;;;                      ;;,;;   ;;;       
 [[[,/[[['  ,[[     \[[,`]]nnn]]',cc[[[cc. [ccc, ,cccc, =[[[[[[.[[[[cc,,. 
 $$$$$$c    $$$,     $$$ $$$""   $$$____   $$$$$$$$"$$$ $$$$$   $$$"""$$$ 
 888b "88bo,"888,_ _,88P 888o     .     88,888 Y88" 888o88888,  888   "88o
 MMMM   "W"   "YMMMMMP"  YMMMb    "YUMMMMP"MMM  M'  "MMMMMMMMM  MMM    YMM
                                                                          
  <br><b>ROP gadget finder & chain generator</b>
</pre>
</div>

## What is ROPsmith?

ROPsmith is a lightweight toolkit focused on discovering ROP gadgets inside binaries (ELF/PE), presenting usable gadgets, and producing starter templates for ROP chains. The goal is educational and practical: help red-teamers, CTF players, and security researchers understand low-level exploit construction while keeping the tool modular and auditable.

> ⚠️ Ethics & usage: Only run ROPsmith against binaries and hosts you own or have explicit permission to test. This project is for research, education, and defensive testing.

## Status

- Stage: Work in progress (MVP: ELF `.text` scanner + basic gadget disassembly)
- Planned features: Capstone integration, gadget ranking heuristics, chain templates, Windows PE support, JSON export, interactive CLI

## Installation

You can execute ROPsmith from one of the pre-compiled binaries available on the [releases page](https://github.com/wh0crypt/ROPsmith/releases). Alternatively, you can build it from source.

### From Source

1. Clone the repository:

```bash
git clone https://github.com/wh0crypt/ROPsmith.git
cd ROPsmith
```

2. Build the project using CMake:

```bash
mkdir build && cd build
cmake .. && make
```

## Usage

- Run the tool with the `-h` flag to see available options:

```bash
./ropsmith -h
```

- Run the tool against a binary:

```bash
./ropsmith /path/to/binary
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributing

See the [CONTRIBUTING.md](.github/CONTRIBUTING.md) file for contribution guidelines.
