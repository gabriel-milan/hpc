program main

  integer, parameter :: n=33000

  ! Allocate resources
  real*8 :: s, rTime, endTime
  real*8, dimension(n, n) :: A
  real*8, dimension(n) :: x, b
  integer :: values(8)

  ! Initialize A and x with random values
  call random_number(A)
  call random_number(x)

  ! Compute b = Ax
  call date_and_time(values=values)
  rTime = (values(5)) * 60.         ! Hours to minutes
  rTime = (rTime + values(6)) * 60. ! Minutes to seconds
  rTime = (rTime + values(7)) * 1e3 ! Seconds to milliseconds
  rTime = rTime + values(8)         ! Add milliseconds
  do i = 1, n
    s = 0
    do j = 1, n
      s = s + A(i,j) * x(j)
    end do
    b(i) = s
  end do
  call date_and_time(values=values)
  endTime = (values(5)) * 60.         ! Hours to minutes
  endTime = (endTime + values(6)) * 60. ! Minutes to seconds
  endTime = (endTime + values(7)) * 1e3 ! Seconds to milliseconds
  endTime = endTime + values(8)         ! Add milliseconds
  print *, endTime - rTime

end program