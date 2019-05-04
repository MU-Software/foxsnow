import code
import contextlib
import io
import sys

@contextlib.contextmanager
def std_redirector(stdin=sys.stdin, stdout=sys.stdin, stderr=sys.stderr):
	"""Temporarily redirect stdin/stdout/stderr"""

	tmp_fds	 = stdin, stdout, stderr
	orig_fds = sys.stdin, sys.stdout, sys.stderr
	sys.stdin, sys.stdout, sys.stderr = tmp_fds
	yield
	sys.stdin, sys.stdout, sys.stderr = orig_fds


class FS_Python_Interpreter(code.InteractiveConsole):
	"""Remote-friendly InteractiveConsole subclass

	This class behaves just like InteractiveConsole, except that it
	returns all output as a string rather than emitting to stdout/stderr

	"""
	banner = ("Python %s\n%s\n" % (sys.version, sys.platform) +
			  'Type "help", "copyright", "credits" or "license" '
			  'for more information.\n')

	ps1 = getattr(sys, "ps1", ">>> ")
	ps2 = getattr(sys, "ps2", "... ")


	def __init__(self, locals=None):
		code.InteractiveConsole.__init__(self, locals=locals)
		self.output = io.StringIO()
		self.error	= io.StringIO()

	def push(self, command):
		"""Return the result of executing `command`

		This function temporarily redirects stdout/stderr and then simply
		forwards to the base class's push() method.	 It returns a 2-tuple
		(more, result) where `more` is a boolean indicating whether the
		interpreter expects more input [similar to the base class push()], and
		`result` is the captured output (if any) from running `command`.

		"""
		self.output.truncate(0)
		self.output.seek(0)
		self.error.truncate(0)
		self.error.seek(0)
		with std_redirector(stdout=self.output, stderr=self.error):
			try:
				more = code.InteractiveConsole.push(self, command)
				result = self.output.getvalue()
				result_err = self.error.getvalue()
			except (SyntaxError, OverflowError):
				pass
			return more, result, result_err

if __name__ == '__main__':
	app = FS_Python_Interpreter(globals())
	no_end, response, error_response = None, None, None
	while True:
		print(app.ps1 if not no_end else app.ps2, end='')
		no_end, response, error_response = app.push(input())
		if error_response:
			print(error_response)
			continue
		print(response)