# --- Common directories ---
core_dir = '../src/core'
utils_dir = '../src/utils'
test_samples_dir = 'test_samples'
scripts_dir = '../scripts'

# --- Generate test samples ---
bash = find_program('bash', required: true)
setup_script = find_program(join_paths(scripts_dir, 'setup_samples.sh'), required: true)

generate_samples = custom_target('generate_samples',
    output: [
        'sample_valid',
        'sample_corrupt',
        'sample_empty',
        'sample_gadgets',
        'sample_no_text',
        'sample_no_elf'
    ],
    command: [bash, setup_script, test_samples_dir],
    build_by_default: true
)

# --- Criterion dependency ---
criterion_dep = dependency('criterion', required: true)

# -- Test sources ---
test_sources = files(
    'test_finder.cpp'
)

# --- Tests executable ---
test_exe = executable(
    'ropsmith_tests',
    test_sources,
    link_with: lib_ropsmith,
    include_directories: incdir,
    dependencies: criterion_dep
)

# --- Set environment variable for tests ---
env = environment()
env.set('TEST_SAMPLES_DIR', test_samples_dir)

# --- Register test ---
test('ROPsmith Tests', test_exe,
    depends: generate_samples,
    workdir: meson.current_source_dir(),
    env: env
)
