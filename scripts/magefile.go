//go:build mage
// +build mage

package main

import (
	"fmt"
	"os"

	"github.com/magefile/mage/sh"
)

// Run will run the program
func Run() error {
	err := Make()
	if err != nil {
		return fmt.Errorf("make: %w", err)
	}

	baseDir, err := os.Getwd()
	if err != nil {
		return fmt.Errorf("getwd: %w", err)
	}
	defer func() {
		os.Chdir(baseDir)
	}()
	err = os.Chdir("../bin/")
	if err != nil {
		return fmt.Errorf("chdir ../bin: %w", err)
	}
	_, err = os.Stat("eqx")
	if err != nil {
		return fmt.Errorf("stat eqx: %w", err)
	}
	err = sh.Run("eqx")
	if err != nil {
		return fmt.Errorf("run eqx: %w", err)
	}
	return nil
}

// Make will compile the program
func Make() error {
	err := CMake()
	if err != nil {
		return fmt.Errorf("cmake: %w", err)
	}

	baseDir, err := os.Getwd()
	if err != nil {
		return fmt.Errorf("getwd: %w", err)
	}
	defer func() {
		os.Chdir(baseDir)
	}()
	os.Chdir("../")

	err = sh.Run("make")
	if err != nil {
		return fmt.Errorf("run make: %w", err)
	}
	return nil
}

// CMake will build a cmake file
func CMake() error {
	baseDir, err := os.Getwd()
	if err != nil {
		return fmt.Errorf("getwd: %w", err)
	}
	defer func() {
		os.Chdir(baseDir)
	}()
	os.Chdir("../")
	_, err = os.Stat("CMakeLists.txt")
	if err != nil && !os.IsExist(err) {
		return fmt.Errorf("stat: %w", err)
	}
	if err == nil {
		return nil
	}

	err = sh.Run("cmake", ".")
	if err != nil {
		return fmt.Errorf("run cmake: %w", err)
	}
	return nil
}
