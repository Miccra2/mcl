class A:
    def __repr__(self) -> str:
        return "Hello from A"

class B:
    a: A = A()
    def __repr__(self) -> str:
        return f"Hello from B and '{self.a=!r}'"

print(B())
