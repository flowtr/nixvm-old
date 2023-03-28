{
	inputs = {
		nixpkgs = {
			url = "nixpkgs/nixos-unstable";
		};
	};
	outputs = {
		self,
		nixpkgs,
	}: let
		system = "x86_64-linux";
		pkgs = nixpkgs.legacyPackages.${system};
	in {
		packages."${system}".default = pkgs.llvmPackages_15.libcxxStdenv.mkDerivation {
			name = "nix-parser";
			src = ./.;
			nativeBuildInputs = with pkgs; [
				byacc
				flex
				cmake
				ninja
				fish
			];
			buildInputs = with pkgs; [
				libcxx
				llvmPackages_15.libllvm
				llvmPackages_15.llvm
			];
			cmakeFlags = [
				"-GNinja"
				"-DCMAKE_BUILD_TYPE=MinSizeRel"
				"-S" ".."
			];
		};
	};
}
