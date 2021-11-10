/* TODO: mandatory part, actually correct result, compact the tester */

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
	eprintf("----------------------------------------\n");
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
void	check_diff(int fd, int a, int b, int *went_wrong)
{
	int	should_exit = do_diff(fd, a, b);
	
	if (a != b)
	{
		eprintf("return value FAIL: printf: %d, ft_printf returns %d.\n", a, b);
		should_exit = 1;
	}
	if (should_exit)
	{
		*went_wrong = 1;
		if (EXIT_WHEN_FAIL)
			m_exit();
	}
}

#define	M_PTEST(line, ...) (\
{\
	eprintf("line %-5d: ", line);\
	a = printf(__VA_ARGS__);\
	\
	b = ft_printf(__VA_ARGS__);\
	fflush(stdout);\
	\
	check_diff(fd, a, b, &failed);\
})

#define PTEST(...) (M_PTEST(__LINE__, __VA_ARGS__))

#define SECTION_PRINT(s) eprintf("=====    %s    =====\n", s)

int	main()
{
	int	fd;
	int	a, b;
	int failed = 0;

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
	eprintf("NOTE: next test is implementation dependent, only should fail if nullptr\n");
	eprintf("is not accounted for(i.e. no (nil), (null), or (null pointer)).\n");
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
	PTEST("%d\n", 122342342423);

	SECTION_PRINT("unsigned integers(%u)");
	PTEST("%u", -1);
	PTEST("%u", 0);
	PTEST("%u", 0x7fffffff);
	PTEST("%u", 2222222222);
	PTEST("%u", 122342342423);
	PTEST("%uuuuu%uuuuuuuujjsdf%ufsdf\n", 2222222222, 1, -4545);

	SECTION_PRINT("throwing a bunch of stuff and seeing what happens");
	PTEST("asdfasdf%%%%pdf%pdfsf%d ddfsdfsdfd%isadf%c\n", &fd, fd, 345, 0);
	PTEST("____%%%XxXXsdfs%x", fd, fd);

	eprintf("\n");
	SECTION_PRINT("BONUS BONUS BONUS BONUS!!!");
	SECTION_PRINT("% d and %+d");
	PTEST("% d% d", 24, -24);
	PTEST("%+d%+d", 24, -24);
	PTEST("% d%+d", 0, 0);
	PTEST("% +d%+ d", 3, 3);
	PTEST("%+ dfsdfn%+ i\n", 24, -24);
	PTEST("___%++++    ++++ ifsdfn%+ + +i", 0x3423424, -24);
	PTEST("mmmmmm\r\7%++++    ++++ ifsdfn%+ + +i", -0x3423424, 54);

	eprintf("NOTE: these tests may fail, as it is technically undefined behaviour.\n");
	eprintf("However, it would be good practice to get these tests to pass.\n");
	PTEST("% s", "");
	PTEST("%+s", "");
	PTEST("% p", "");
	PTEST("%+p", "");

	SECTION_PRINT("# hash flag(prepending 0x)");
	PTEST("%#x", 12345);
	PTEST("%#X", 12354);
	PTEST("%#x", 0);
	PTEST("%#x%X", 1, 0xbaaaa);
	PTEST("     ffsdf%#################Xdffff%##############xdfddfsdfsdfs", 0xfe012c, 0xfaef);

	SECTION_PRINT("basic mimimum field width(%5s)");
	PTEST("%5d", 2);
	PTEST("%1d", 2);
	PTEST("%0d", 2);
	PTEST("%256d", 244);
	PTEST("%5d", 22222222);
	PTEST("%5s", "hi");
	PTEST("%5s", "haiiii!!! UwU");
	PTEST("%3c", 0);

	SECTION_PRINT("basic prepended min. field width(%-5s))");
	PTEST("%-5d", 2);
	PTEST("%-1d", 2);
	PTEST("%-0d", 2);
	PTEST("%-129d", 244);
	PTEST("%-5d", 22222222);
	PTEST("%-5s", "hi");
	PTEST("%-5s", "haiiii!!! UwU");
	PTEST("%-3c", 0);

	SECTION_PRINT("min. field width with %+d and % d and %#x");
	PTEST("%+5d, %+5d", 22, -22);
	PTEST("% 4d", 22);
	PTEST("%#5x%#5X", 33, 0x4f);
	PTEST("%#5x%#5X", 0xfefefefe, 0x4ffffff);
	PTEST("%+-5d, %+-5d", 22, -22);
	PTEST("% -5d", 22);
	PTEST("%#-5x%#-5X", 33, 0x4f);
	PTEST("%#-5x%#-5X", 0xfefefefe, 0x4ffffff);

	SECTION_PRINT("min. field width with 0 flag");
	PTEST("%05d", 5);
	PTEST("%05d", 512);
	PTEST("%05d", -5);
	PTEST("%05d", -555);
	PTEST("%05d", -5556);
	PTEST("%05d", -55569);
	PTEST("%0000005s", "hi");
	PTEST("%0-5d", 5);
	PTEST("%-05d", 512);
	PTEST("%0-5d", -5);
	PTEST("%0-5d", -555);
	PTEST("%0-5d", -5556);
	PTEST("%0-5d", -55569);
	PTEST("%0000-05s", "hi");

	/* this took me a while to fix for my own printf... xD */
	SECTION_PRINT("min. field width with 0 and %+ d or %x");
	PTEST("% 05d", 5);
	PTEST("%0+5d", 51);
	PTEST("%+05i", 0x22);
	PTEST("% 05d", 5);
	PTEST("% 05d", -12);
	PTEST("% 05d", -3412);
	PTEST("%+05d", -34129);
	PTEST("%0#7x", 0xaef);
	PTEST("%#07X", 0x2a7);
	PTEST("%25p", &fd);

	SECTION_PRINT("basic precision %.<int>d");
	PTEST("%.4d", -4);
	PTEST("%.4d", 55555);
	PTEST("%.4d", 55);
	PTEST("%.d", 5);
	PTEST("%.0d", 5);
	PTEST("%.4x", -4);
	PTEST("%.4X", 55555);
	PTEST("%.4X", 55);
	PTEST("%.x", 5);
	PTEST("%5.4d", -4);
	PTEST("%5.4d", 4);
	PTEST("%5.4d", 0);
	PTEST("%5.4d", 234235);
	PTEST("%5.4i", 52);
	PTEST("%5.4u", 52);
	PTEST("%5.u", 52);
	PTEST("%5.654u", 52);
	PTEST("%2.1d", -1);
	PTEST("%5.4c", 'h');

	SECTION_PRINT("basic precision with %s");
	PTEST("%.s", "hi");
	PTEST("%.0s", "hi");
	PTEST("%.1s", "hi");
	PTEST("%.4s", "hi");
	PTEST("%.3s", "hello!");

	SECTION_PRINT("precision with 0#-+ flag");
	PTEST("%06d.4", -56);
	PTEST("%+06d.4", 956);
	PTEST("%+-06d.4", 95666);
	PTEST("%+-6d.4", 96);
	PTEST("% -06d.4", 95666);
	PTEST("% -6d.4", 96);
	PTEST("% -06d.1", 95666);
	PTEST("% -6d.1", 96);
	PTEST("%#x-6d.1", 96);
	PTEST("%#x-6.1d", 96456345);

	PTEST("% 6.4d", -56);
	PTEST("%+6.4d", 956);
	PTEST("%09.4d", -56);
	PTEST("%+09.4d", 956);

	PTEST("%+-06.4d", 95666);
	PTEST("%+-6.4d", 96);
	PTEST("% -06.4d", 95666);
	PTEST("% -6.4d", 96);
	PTEST("% -06.1d", 95666);
	PTEST("% -6.1d", 96);
	PTEST("%#x-6.1d", 96);
	PTEST("%#x-6.1d", 96456345);
	PTEST("%#-6.1x", 96);
	PTEST("%#-6.1x", 96456345);

	PTEST("%8.3s", "hai!");
	PTEST("%-8.3s", "hai!");

	SECTION_PRINT("a bunch of stuff at once");
	PTEST("%+-16.5d_\n", 2555559);
	PTEST("%+-16.5d_\n", -2555559);
	PTEST("%0-15.5d_\n", 559);
	PTEST("%0-15.5d %-15p XD\n", 559, &fd);
	PTEST("I like %%%#12x_and_%-4.3s\n", 0xfa1afe1, "pizza");

	close(fd);
	if (failed)
		m_exit();
	eprintf("-----------------\n");
	eprintf("all OK! good job!\n");
	return (0);
}
