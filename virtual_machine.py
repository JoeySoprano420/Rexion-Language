vm = RexionFullVM(debug=True)
vm.load_binary("/mnt/data/rexion_output.bin")
vm.execute()
vm.show_state()
