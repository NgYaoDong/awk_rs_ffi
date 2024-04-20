set -e
cargo build --no-default-features
env -i VERBOSE=true ./awk_rs.tests target/debug/awk_rs > awk_rs.test.result.verbose || true
env -i ./awk_rs.tests target/debug/awk_rs > awk_rs.test.result || true
