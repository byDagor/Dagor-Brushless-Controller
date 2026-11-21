// Overrides the core's weak verifyRollbackLater(), which has C linkage.
// Returning true keeps a freshly updated firmware image "pending verification" instead of validating it at start-up.
// The sketch validates it after a healthy start (m_ROLLBACK.ino). If the board restarts before that,
// the bootloader goes back to the previous image.

extern "C" bool verifyRollbackLater() {
  return true;
}
