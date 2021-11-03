#include <stdio.h> /* printf, obviously */
#include <stdlib.h> /* exit */
#include <string.h> /* memset, strncmp */

#include <unistd.h> /* dup, read */
#include <sys/stat.h> /* redir */
#include <fcntl.h> /* fchmod etc. */

int	ft_printf(const char *format, ...);

#define EXIT_WHEN_FAIL (0) // set to 1 if you want the test to stop when it fails.
#define TEST_OUTPUT_FILE_NAME ("output.txt") // File name to write results to

// the buffer size. If a printf tests exceeds this buffer, idk what happens.
#define BUF_SIZE (4096)

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

// prints a kind message, then exits
void	m_exit()
{
	eprintf("Mission failed, we'll get 'em next time!\n");
	exit(1);
}

// perrors and exits
void	m_fatal_error(const char *s)
{
	perror(s);
	exit(1);
}

void	ptest_init(int *fd)
{
	*fd = open(TEST_OUTPUT_FILE_NAME, O_RDWR | O_CREAT | O_APPEND | O_TRUNC);
	if (*fd == -1)
	{
		m_fatal_error("open");
	}
	// change perms so we can edit it later
	if (fchmod(*fd, 0644) == -1)
	{
		perror("fchmod");
	}
	// redirect stdout to the file
	if (dup2(*fd, 1) == -1)
	{
		m_fatal_error("dup2");
	}
}

int	do_diff(int fd, int a, int b)
{
	char	buf_a[BUF_SIZE];
	char	buf_b[BUF_SIZE];
	ssize_t	i;


	memset(buf_a, 0, BUF_SIZE);
	memset(buf_b, 0, BUF_SIZE);
	if (lseek(fd, (off_t)(-(a + b)), SEEK_END) == -1)
	{
		m_fatal_error("lseek");
	}

	i = read(fd, buf_b, b);
	if (i == -1)
	{
		m_fatal_error("read");
	}
	else if (i != b)
	{
		eprintf("expected read failed: expected %d, got %zd\n", b, i);
	}
	i = read(fd, buf_a, a);
	if (i == -1)
	{
		m_fatal_error("read");
	}
	else if (i != a)
	{
		eprintf("expected read failed: expected %d, got %zd\n", a, i);
	}
	if (strncmp(buf_a, buf_b, BUF_SIZE) != 0)
	{
		eprintf("FAIL:\n\nprintf:    %s\nft_printf: %s\n\n", buf_a, buf_b);
		return (1);
	}
	eprintf("OK\n");
	return (0);
}

/* checks the difference between the two printf calls just done */
void	check_diff(int fd, int a, int b)
{
	int	should_exit = do_diff(fd, a, b);
	
	if (a != b)
	{
		eprintf("return value FAIL: printf: %d, ft_printf returns %d.\n", a, b);
		should_exit = 1;
	}
	if (should_exit && EXIT_WHEN_FAIL)
		m_exit();
}

#define	M_PTEST(line, ...) (\
{\
	eprintf("line %-5d: ", line);\
	a = printf(__VA_ARGS__);\
	\
	b = ft_printf(__VA_ARGS__);\
	fflush(stdout);\
	\
	check_diff(fd, a, b);\
})

#define PTEST(...) (M_PTEST(__LINE__, __VA_ARGS__))

#define SECTION_PRINT(s) eprintf("=====    %s    =====\n", s)

int	main()
{
	int	fd;
	int	a, b;

	SECTION_PRINT("let the PRINTF TESTING commence! ");
	ptest_init(&fd);

	SECTION_PRINT("just literal string");
	PTEST("");
	PTEST("HALLO");
	PTEST("HELLO DOES THIS WORK?");
	PTEST("unexpected \0, what will happen?");

	SECTION_PRINT("% screwery");
	PTEST("%%\n");
	PTEST("%");
	PTEST("%%%%%%\n");
	PTEST("%%%%%%%");
	PTEST("%%%%%%%\n");

	//SECTION_PRINT("basic invalid arguments");
	//PTEST("hoi %S\n", "hi");

	SECTION_PRINT("%c stuff");
	PTEST("%c", 'h');
	PTEST("%c", -1);
	PTEST("sdf sdfsdf sd %c\t\v%c", 'h', 'a');
	PTEST("%c%c\n", '\0', '~');

	SECTION_PRINT("strings(%s)");
	PTEST("hello, %s\n", "world");
	PTEST("%s%s", "h", "\n");
	PTEST("%s%s\n", "sdfd\0fs", "asdfasdf");
	PTEST("hello, %s\n", "\0");
	eprintf("NOTE: next test is implementation dependent, only should fail if nullptr is not accounted for.\n");
	PTEST("super cool string: %s", (char *)0);

	SECTION_PRINT("pointers(%p)");
	PTEST("cool pointer: %p\n", &fd);
	PTEST("cool pointer: %p\n", (void *)0);
	PTEST("%p\n", (void *)-1);
	PTEST("a:%pb:%p", &a, &b);

	SECTION_PRINT("hexadecimal(%x %X))");
	PTEST("%x", -1);
	PTEST("%x", 0);
	PTEST("%x", 0xfa1afe);
	PTEST("%x%x", 128998, 0);
	PTEST("%X", -1);
	PTEST("%X", 0);
	PTEST("%X", 0xbafefe);
	PTEST("%x  %X ", 0xba, 0xef);


	SECTION_PRINT("signed integers(%d and %i)");
	PTEST("%d\n", 3450);
	PTEST("%d\n", -3450);
	PTEST("%d\n", 0x7fffffff);
	PTEST("%d\n", 0x80000000);
	PTEST("%d\n", 0);
	PTEST("%d\n", -1);
	PTEST("%i\n", 3450);
	PTEST("%i\n", -3450);
	PTEST("%i\n", 0x7fffffff);
	PTEST("%i\n", 0x80000000);
	PTEST("%i\n", 0);
	PTEST("%i\n", -1);
	PTEST("%isadfsadf%d", -1, 34);
	PTEST("\tadswfdsf%ifdf%ddfdfsf", 0, 0xfa1afe1);

	SECTION_PRINT("BONUS BONUS BONUS BONUS!!!");
	SECTION_PRINT("% d and %+d");
	PTEST("% d% d", 24, -24);
	PTEST("%+d%+d", 24, -24);
	PTEST("% +d%+ d", 2, 2);
	PTEST("%+ dfsdfn%+ i\n", 24, -24);
	PTEST("___%++++    ++++ ifsdfn%+ + +i", 0x3423424, -24);
	PTEST("mmmmmm\r\7%++++    ++++ ifsdfn%+ + +i", -0x3423424, 54);

	SECTION_PRINT("# hash flag(prepending 0x)");
	PTEST("%#x", 12345);
	PTEST("%#X", 12354);
	PTEST("%#x", 0);
	PTEST("%#x%X", 1, 0xbaaaa);
	PTEST("     ffsdf%#################Xdffff%##############xdfddfsdfsdfs", 0xfe012c, 0xfaef);


	/*
	PTEST("asdf asdf asdf%pdfdfsf%dddfsdfsdfd%isadf\n", &fd, fd, 345);
	PTEST("%u\n", 0x80000001);
	PTEST("%x%X\n", 0x73434, -234946);
	 */

	close(fd);
	eprintf("-----------------\n");
	eprintf("all OK! good job!\n");
	return (0);
}
