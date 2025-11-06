# Directory for test samples
test_samples_dir = join_paths(meson.project_source_root(), 'tests', 'test_samples')

# --- Generate test samples ---
bash = find_program('bash', required : true)
setup_script = find_program('../scripts/setup_samples.sh', required : true)

generate_samples = custom_target('generate_samples',
    output : [
        'sample_valid',
        'sample_corrupt',
        'sample_empty',
        'sample_gadgets',
        'sample_no_text',
        'sample_no_elf'
    ],
    command : [bash, setup_script, test_samples_dir],
    build_by_default : true
)

# --- Criterion dependency ---
criterion_dep = dependency('criterion', required : true)

# --- Tests executable ---
test_exe = executable(
    'ropsmith_tests',
    files('test_finder.c'),
    include_directories : inc,
    link_with : lib_ropsmith,
    dependencies : criterion_dep,
    cpp_args : ['-DMESON_SOURCE_ROOT=\\"' + meson.current_source_dir() + '\\"']
)

# --- Set environment variable for tests ---
env = environment()
env.set('MESON_SOURCE_ROOT', meson.current_source_dir())

# --- Register test ---
test('ROPsmith Tests', test_exe,
    depends: generate_samples,
    workdir: meson.current_source_dir(),
    env : env
)
